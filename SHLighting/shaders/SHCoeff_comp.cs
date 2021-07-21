#version 450 core

#define PI 3.1415926535897
#define RCP_PI 0.318309886183

#define THREAD_X 8
#define THREAD_Y 8
#define SH_DEGREE 3

const uint shcCount = SH_DEGREE * SH_DEGREE;

//ÿ���߳����ڲ���ÿ���̼߳���Ľ����������������У���СΪ8*8*9
shared vec4 shcGroup[THREAD_X * THREAD_Y * SH_DEGREE * SH_DEGREE]; 

uniform vec2 SampleSize;

uniform samplerCube skybox;

layout(local_size_x = THREAD_X, local_size_y = THREAD_Y, local_size_z = 1) in;

layout(std430, binding = 0) buffer sh_coeff_buffer
{
	vec4 shcBuffer[];
};

//spherical coordinate sh function
//l = 0,m = 0
float GetY00(float theta, float phi)
{
	return 0.5 * sqrt(RCP_PI);
}

//l = 1,m = 0
float GetY10(float theta, float phi)
{
	return 0.5 * sqrt(3 * RCP_PI) * cos(theta);
}

//l = 1,m = 1
float GetY1p1(float theta, float phi)
{
	return 0.5 * sqrt(3 * RCP_PI) * sin(theta) * cos(phi);
}

//l = 1,m = -1
float GetY1n1(float theta, float phi)
{
	return 0.5 * sqrt(3 * RCP_PI) * sin(theta) * sin(phi);
}

//l = 2, m = 0
float GetY20(float theta, float phi)
{
	float c = cos(theta);
	return 0.25 * sqrt(5 * RCP_PI) * (3 * c * c - 1);
}

//l = 2, m = 1
float GetY2p1(float theta, float phi)
{
	return 0.5 * sqrt(15 * RCP_PI) * sin(theta) * cos(theta) * cos(phi);
}

//l = 2, m = -1
float GetY2n1(float theta, float phi)
{
	return 0.5 * sqrt(15 * RCP_PI) * sin(theta) * cos(theta) * sin(phi);
}

//l = 2, m = 2
float GetY2p2(float theta, float phi)
{
	float s = sin(theta);
	return 0.25 * sqrt(15 * RCP_PI) * s * s * cos(2 * phi);
}

//l = 2, m = -2
float GetY2n2(float theta, float phi)
{
	float s = sin(theta);
	return 0.25 * sqrt(15 * RCP_PI) * s * s * sin(2 * phi);
}


void GetThetaPhi(uint x, uint y, out float theta, out float phi)
{
	theta = acos(1 -  x * 2 / (SampleSize.x - 1));
	phi = 2 * PI * (y * 1 / (SampleSize.y - 1));
}

vec4 cal_test_light(float theta, float phi)
{
	float res = max(0.0, 5 * cos(theta) - 4) + max(0.0, -4 * sin(theta - PI) * cos(phi - 2.5) - 3);

	return vec4(res, res, res, 1.0);
}

void CalSH(float theta, float phi, uint groupIndex)
{
    float y = cos(phi);
    float x = sin(theta) * sin(phi);
    float z = cos(theta) * sin(phi);
    vec4 color = texture(skybox, vec3(x, y, z));
	float N = SampleSize.x * SampleSize.y;
	float A = 4 * PI / N;

	uint groupOffset = groupIndex * shcCount;
	shcGroup[groupOffset + 0] = color * GetY00(theta, phi) * A;
	shcGroup[groupOffset + 1] = color * GetY1n1(theta, phi) * A;
	shcGroup[groupOffset + 2] = color * GetY10(theta, phi) * A;
	shcGroup[groupOffset + 3] = color * GetY1p1(theta, phi) * A;
	shcGroup[groupOffset + 4] = color * GetY2n2(theta, phi) * A;
	shcGroup[groupOffset + 5] = color * GetY2n1(theta, phi) * A;
	shcGroup[groupOffset + 6] = color * GetY20(theta, phi) * A;
	shcGroup[groupOffset + 7] = color * GetY2p1(theta, phi) * A;
	shcGroup[groupOffset + 8] = color * GetY2p2(theta, phi) * A;
}

void main()
{
	//where this thread is in the all work items
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;

    //where this thread is in the current work group (1D representation)
    uint groupIndex = gl_LocalInvocationIndex;

    float theta, phi;
    GetThetaPhi(x, y, theta, phi);

    CalSH(theta, phi, groupIndex);

    groupMemoryBarrier();

    uint threadCount = THREAD_X * THREAD_Y;

    //shcGroup���߳����ڹ��������
    //GPGPU �Ӻ��㷨����ÿ���߳�������õ���ֵ�ӵ�shcGroup��ȥ�����ַ��Ӻ���һ�μӵ�ǰ��һ��
    for (uint k = (threadCount >> 1); k > 0; k >>= 1)
    {
        if (groupIndex < k)
        {
            uint shIndex = groupIndex * shcCount;
            uint shIndex2 = (groupIndex + k) * shcCount;
            for (uint offset = 0; offset < shcCount; offset++)
            {
                shcGroup[shIndex + offset] += shcGroup[shIndex2 + offset];
            }
        }
        groupMemoryBarrier();
    }

    //���߳���Ľ��д��buffer�У����ظ�CPU�������
    //�����߳���ĵ�һ���߳̽���д����
    if (groupIndex == 0)
    {
        uint groupCountX = gl_NumWorkGroups.x;
        uint index = (gl_WorkGroupID.y * groupCountX + gl_WorkGroupID.x) * shcCount;
        for (uint i = 0; i < shcCount; i++)
        {
            vec4 c = shcGroup[i];
            shcBuffer[index + i] = c;
        }
    }
}
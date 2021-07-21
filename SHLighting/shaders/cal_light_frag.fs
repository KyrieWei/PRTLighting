#version 450 core
#define PI 3.14159265358979323846

in vec3 normal;

out vec4 fragColor;

float sh_basis[16];

layout (std140, binding = 0) uniform sh_coeff_buffer
{
	vec4 sh_coeff[];
};

void generate_basis(float x, float y, float z)
{
	sh_basis[0] = 0.5 * sqrt(1.0 / PI);
	sh_basis[1] = -sqrt(3 / (4 * PI)) * y;
	sh_basis[2] = sqrt(3 / (4 * PI)) * z;
	sh_basis[3] = -sqrt(3 / (4 * PI)) * x;
	sh_basis[4] = 0.5 * sqrt(15 / PI) * x * y;
	sh_basis[5] = -0.5 * sqrt(15 / PI) * y * z;
	sh_basis[6] = 0.25 * sqrt(5 / PI) * (-x * x - y * y + 2 * z * z);
	sh_basis[7] = -0.5 * sqrt(15 / PI) * z * x;
	sh_basis[8] = 0.25 * sqrt(15 / PI) * (x * x - y * y);
	sh_basis[9] = -0.25 * sqrt(35 / (2 * PI)) * (3 * x * x - y * y) * y;
	sh_basis[10] = 0.5 * sqrt(105 / PI) * x * y * z;
	sh_basis[11] = -0.25 * sqrt(21 / (2 * PI)) * y * (4 * z * z - x * x - y * y);
	sh_basis[12] = 0.25 * sqrt(7 / PI) * z * (2 * z * z - 3 * x * x - 3 * y * y);
	sh_basis[13] = -0.25 * sqrt(21 / (2 * PI)) * x * (4 * z * z - x * x - y * y);
	sh_basis[14] = 0.25 * sqrt(105 / PI) * (x * x - y * y) * z;
	sh_basis[15] = -0.25 * sqrt(35 / (2 * PI)) * (x * x - 3 * y * y) * x;
}

void main()
{
    float theta = acos(normal.z);
    float phi = acos(normal.x / sin(theta));
	
	generate_basis(normal.x, normal.y, normal.z);

    vec4 light = sh_coeff[0] * sh_basis[0] +
				 sh_coeff[1] * sh_basis[1] + sh_coeff[2] * sh_basis[2] + sh_coeff[3] * sh_basis[3] +
				 sh_coeff[4] * sh_basis[4] + sh_coeff[5] * sh_basis[5] + sh_coeff[6] * sh_basis[6] + sh_coeff[7] * sh_basis[7] + sh_coeff[8] * sh_basis[8];

    fragColor = vec4(light.xyz, 1.0);
}
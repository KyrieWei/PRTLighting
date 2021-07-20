#version 450 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (rgba32f, binding = 0) uniform image2D toTex;

uniform vec2 SampleSize;

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

vec3 get_color()
{
    return vec3(0.3, 0.5, 0.8);
}


void main()
{
    ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

    vec3 color = get_color();
    vec4 pixel = vec4(color, 1.0);

    imageStore(toTex, texelCoords, pixel);
}
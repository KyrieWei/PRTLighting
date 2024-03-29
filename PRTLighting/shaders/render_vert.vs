#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

void main()
{
	Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
	TexCoord = aTexCoord;
	WorldPos = vec3(model * vec4(aPos, 1.0));
	FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);

	gl_Position = projection * view * vec4(WorldPos, 1.0);
}
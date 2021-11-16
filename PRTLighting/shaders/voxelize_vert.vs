#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
out vec3 GeomNormal;

void main()
{
	GeomNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
	gl_Position = model * vec4(aPos, 1.0f);
}
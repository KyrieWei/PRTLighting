#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 GeomNormal[];

out vec3 FragPos;
out vec3 FragNormal;

uniform mat4 viewProject[3];

uint selectViewProject()
{
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 faceNormal = cross(p1, p2);
	float nDX = abs(faceNormal.x);
	float nDY = abs(faceNormal.y);
	float nDZ = abs(faceNormal.z);

	if(nDX > nDY && nDX > nDZ)
	{
		return 0;
	}
	else if(nDY > nDX && nDY > nDZ)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

void main()
{
	uint projectIndex = selectViewProject();
	FragPos = gl_in[0].gl_Position.xyz;
	FragNormal = GeomNormal[0];
	gl_Position = viewProject[projectIndex] * gl_in[0].gl_Position;
	EmitVertex();
	
	FragPos = gl_in[1].gl_Position.xyz;
	FragNormal = GeomNormal[1];
	gl_Position = viewProject[projectIndex] * gl_in[1].gl_Position;
	EmitVertex();

	FragPos = gl_in[2].gl_Position.xyz;
	FragNormal = GeomNormal[2];
	gl_Position = viewProject[projectIndex] * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}
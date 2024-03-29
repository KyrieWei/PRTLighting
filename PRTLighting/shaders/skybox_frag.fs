#version 450 core

out vec4 fragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    fragColor = textureLod(skybox, TexCoords, 1.0);
}
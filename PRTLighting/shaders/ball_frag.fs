#version 450 core

in vec3 normal;

out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
    vec4 color = texture(skybox, normal);
    fragColor = color;
}
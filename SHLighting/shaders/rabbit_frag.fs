#version 330 core

in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D earthmapTexture;

void main()
{
    vec4 color = texture(earthmapTexture, texCoord);
    fragColor = color;
}
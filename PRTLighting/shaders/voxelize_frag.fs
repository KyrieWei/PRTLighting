#version 450 core

in vec3 FragPos;
in vec3 FragNormal;

//lighting settings
#define POINT_LIGHT_INTENSITY 1
#define MAX_LIGHTS 1

//lighting attenuation factors
#define DIST_FACTOR 1.1f
#define CONSTANT 1
#define LINEAR 0
#define QUADRATIC 1

struct PointLight
{
    vec3 position;
    vec3 color;
};

struct Material
{
    vec3 diffuseColor;
    vec3 specularColor;
    float diffuseReflectivity;
    float specularReflectivity;
    float emissivity;
};

uniform vec3 boxMin;
uniform vec3 scale;

uniform int numberOfLights;
uniform vec3 cameraPosition;

uniform Material material;
uniform PointLight pointLights[MAX_LIGHTS];

layout(RGBA8) uniform image3D texture3D;

float attenuate(float dist)
{
    dist *= DIST_FACTOR;
    return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist);
}

vec3 calculatePointLight(PointLight light)
{
    vec3 direction = normalize(light.position - FragPos);
    float distanceToLight = distance(light.position, FragPos);
    float attenuation = attenuate(distanceToLight);
    float d = max(dot(normalize(FragNormal), direction), 0.0f);
    return d * POINT_LIGHT_INTENSITY * attenuation * light.color;
}


void main()
{
    //calculate diffuse lighting fragment contribution
    vec3 color = vec3(0.0f);

    const uint maxLights = min(numberOfLights, MAX_LIGHTS);
    for(uint i = 0; i < maxLights; i++)
        color += calculatePointLight(pointLights[i]);

    vec3 spec = material.specularReflectivity * material.specularColor;
    vec3 diff = material.diffuseReflectivity * material.diffuseColor;
    color = (diff + spec) * color + clamp(material.emissivity, 0, 1) * material.diffuseColor;

    vec3 curr = FragPos - boxMin;
    ivec3 coord = ivec3(curr * scale);

    ivec3 dim = imageSize(texture3D);

    coord.x = clamp(coord.x, 0, dim.x - 1);
    coord.y = clamp(coord.y, 0, dim.y - 1);
    coord.z = clamp(coord.z, 0, dim.z - 1);

    vec4 res = vec4(color, 1.0);

    imageStore(texture3D, coord, res);
}

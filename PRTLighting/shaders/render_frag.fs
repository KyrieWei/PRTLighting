#version 450 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 direction;
    vec3 radiance;
};

//textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;

uniform sampler2D texture_depth;

//Light
uniform DirectionalLight directionalLight;

uniform vec3 viewPos;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx2 * ggx1;
}

float NormalDistributionGGX(vec3 N, vec3 H, float roughness)
{

}

float ShadowCalculate(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    //clip space coord perform /w to NDC coord
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(texture_depth, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(texture_depth, 0);
    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; y++)
        {
            float pcfDepth = texture(texture_depth, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 49;

    return shadow;
}

void main()
{
    vec3 normal = normalize(texture(texture_normal, TexCoord).rgb - vec3(1.0f));
    vec3 albedo = pow(texture(texture_diffuse, TexCoord).rgb, vec3(2.2));
    //sample roughness
    float roughness = texture(texture_roughness, TexCoord).r;
    //sample metallic
    float metallic = texture(texture_metallic, TexCoord).r;

    
    vec3 viewDir = normalize(viewPos - WorldPos);
    //index of refracted
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    //------------------------directional light----------------------------
    vec3 lightDir = directionalLight.direction;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    //fresnel factor
    vec3 fresnel = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);
    //normal distribution factor
    float distribution = NormalDistributionGGX(normal, halfwayDir, roughness);
    //geometry factor
    float geometryFactor = GeometrySmith(normal, viewDir, roughtness);
    //brdf function
    vec3 brdf = distribution * fresnel * geometryFactor / (4.0f * max(dot(viewDir, normal), 0.0f) * max(dot(lightDir, normal), 0.0f) + 0.0001f);
    vec3 kSpecular = fresnel;
    vec3 kDiffuse = vec3(1.0f) - kSpecular;
    kDiffuse *= (1.0f - metallic);
    //rendering equation
    FragColor.rgb = (kDiffuse * albedo / PI + brdf) * dirLight.radiance * max(dot(normal, lightDir), 0.0f);
    //--------------------------------------------------------------------

    //vec3 ambient = directionalLight.radiance * 0.3;

    //float diff = max(dot(Normal, lightDir), 0.0);
    //vec3 diffuse = directionalLight.radiance * diff;

    //float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32);
    //vec3 specular = directionalLight.radiance * spec;

    ////shadow
    //float shadow = ShadowCalculate(FragPosLightSpace, Normal, lightDir);

    //vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;

    //FragColor = vec4(color, 1.0);
}
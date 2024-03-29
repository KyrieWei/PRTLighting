#version 450 core

const float PI = 3.14159265359;

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

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texture_normal, TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

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
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
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
    vec3 normal = getNormalFromMap();

    vec3 albedo = pow(texture(texture_diffuse, TexCoord).rgb, vec3(2.2));
    //sample roughness
    float roughness = texture(texture_roughness, TexCoord).r;
    //sample metallic
    float metallic = texture(texture_metallic, TexCoord).r;

    vec3 viewDir = normalize(viewPos - WorldPos);
    vec3 color = vec3(0.0f);

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
    float geometryFactor = GeometrySmith(normal, viewDir, lightDir, roughness);
    //brdf function
    vec3 brdf = distribution * fresnel * geometryFactor / (4.0f * max(dot(viewDir, normal), 0.0f) * max(dot(lightDir, normal), 0.0f) + 0.0001f);
    vec3 kSpecular = fresnel;
    vec3 kDiffuse = vec3(1.0f) - kSpecular;
    kDiffuse *= (1.0f - metallic);
    //rendering equation
    FragColor.rgb = (kDiffuse * albedo / PI + brdf) * directionalLight.radiance * max(dot(normal, lightDir), 0.0f);
    //--------------------------------------------------------------------

    //vec3 ambient = vec3(0.03) * albedo;
    //color += ambient;

    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));
    
    //FragColor = vec4(color, 1.0);

    vec3 ambient = directionalLight.radiance * 0.03;

    //float diff = max(dot(Normal, lightDir), 0.0);
    //vec3 diffuse = directionalLight.radiance * diff;

    //float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32);
    //vec3 specular = directionalLight.radiance * spec;

    ////shadow
    float shadow = ShadowCalculate(FragPosLightSpace, Normal, lightDir);

    FragColor.rgb = ambient + FragColor.rgb * shadow;
}
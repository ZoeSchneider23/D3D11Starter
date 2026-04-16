#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

static const float MIN_ROUGHNESS = 0.0000001;
static const float PI = 3.14159265f;
// ALL of your code pieces (structs, functions, etc.) go here!
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float time : TIME;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
};

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float time : TIME;
    float3 tangent : TANGENT;
};

struct Light
{
    int type;
    float3 direction;
    float range;
    float3 position;
    float intensity;
    float3 color;
    float spotInnerAngle;
    float spotOuterAngle;
    float2 padding;
};

struct SkyVertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

float D_GGX(float3 n, float3 h, float roughness)
{
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness; // Remap roughness (Unreal & Disney)
    float a2 = max(a * a, MIN_ROUGHNESS); // MIN_ROUGHNESS is 0.0000001
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    
    return a2 / (PI * denomToSquare * denomToSquare);
}

float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


float G_SchlickGGX_Optimized(float3 n, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    return 1 / (NdotV * (1 - k) + k);
}

float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0)
{
    float3 h = normalize(v + l);
    // Grab various functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G =
    G_SchlickGGX_Optimized(n, v, roughness) *
    G_SchlickGGX_Optimized(n, l, roughness);
    // Final formula
    return ((D * F * G) / 4) * saturate(dot(n, l));
}

float DiffuseEnergyConserve(
float diffuse,
float3 F, // Should be result of just F_Schlick()
float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

float4 DirectionalLight(VertexToPixel input, Light directionalLight, float4 surfaceColor, float3 cameraPosition, float roughness, float metalness, float3 specularColor)
{
    
    float3 dirToCamera = normalize(input.worldPosition - cameraPosition);
    
    float diff = saturate(dot(input.normal, directionalLight.direction));
    
    float3 spec = MicrofacetBRDF(input.normal, directionalLight.direction, -dirToCamera, roughness, specularColor);
    //spec *= any(diff);

    float3 F = F_Schlick(-dirToCamera, normalize(directionalLight.direction - dirToCamera), specularColor);
    float balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return ((balancedDiff * surfaceColor) + spec.rgbb) * directionalLight.intensity * directionalLight.color.rgbb;
}

float4 PointLight(VertexToPixel input, Light light, float4 surfaceColor, float3 cameraPosition, float roughness, float metalness, float3 specularColor)
{
    float3 dirToCamera = normalize(input.worldPosition - cameraPosition);
    float3 lightDirection = normalize(light.position - input.worldPosition);
    
    float diff = saturate(dot(input.normal, lightDirection));
    
    float3 spec = MicrofacetBRDF(input.normal, lightDirection, -dirToCamera, roughness, specularColor);
    //spec *= any(diff);

    float3 F = F_Schlick(-dirToCamera, normalize(lightDirection - dirToCamera), specularColor);
    float balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    return ((balancedDiff * surfaceColor) + spec.rgbb) * light.intensity * light.color.rgbb;
}

float4 SpotLight(VertexToPixel input, Light light, float4 surfaceColor, float3 cameraPosition, float roughness, float metalness, float3 specularColor)
{
    float4 baseLight = PointLight(input, light, surfaceColor, cameraPosition, roughness, metalness, specularColor);
    
    float3 lightToPixel = normalize(light.position - input.worldPosition);
    float pixelAngle = saturate(dot(lightToPixel, light.direction));
    float cosOuter = cos(light.spotOuterAngle);
    float cosInner = cos(light.spotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return baseLight * spotTerm;
}

#endif
#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
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


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

float4 DirectionalLight(VertexToPixel input, Light directionalLight, float4 surfaceColor, float3 cameraPosition)
{
    float3 dirToCamera = normalize(input.worldPosition - cameraPosition);
    
    float3 diffuseTerm = saturate(dot(input.normal, directionalLight.direction)) * // Diffuse intensity, clamped to 0-1
        directionalLight.color * directionalLight.intensity * // Light’s overall color
        surfaceColor.rgb;
    
    float3 refl = reflect(directionalLight.direction, input.normal);
    float RdotV = saturate(dot(refl, dirToCamera));
    float3 specularTerm =
        pow(RdotV, 128) * directionalLight.color * directionalLight.intensity * surfaceColor.rgb;
    specularTerm *= any(diffuseTerm);

    return diffuseTerm.rgbb + specularTerm.rgbb;
}

float4 PointLight(VertexToPixel input, Light light, float4 surfaceColor, float3 cameraPosition)
{
    float3 dirToCamera = normalize(input.worldPosition - cameraPosition);
    float3 lightDirection = normalize(input.worldPosition - light.position);
    
    float3 diffuseTerm = saturate(dot(input.normal, -lightDirection)) * // Diffuse intensity, clamped to 0-1
        light.color * light.intensity * // Light’s overall color
        surfaceColor.rgb;
    
    float3 refl = reflect(-lightDirection, input.normal);
    float RdotV = saturate(dot(refl, dirToCamera));
    float3 specularTerm =
        pow(RdotV, 128) * light.color * light.intensity * surfaceColor.rgb;
    specularTerm *= any(diffuseTerm);
    
    return (diffuseTerm.rgbb + specularTerm.rgbb) * Attenuate(light, input.worldPosition);
}

float4 SpotLight(VertexToPixel input, Light light, float4 surfaceColor, float3 cameraPosition)
{
    float4 baseLight = PointLight(input, light, surfaceColor, cameraPosition);
    
    float3 lightToPixel = normalize(light.position - input.worldPosition);
    float pixelAngle = saturate(dot(lightToPixel, light.direction));
    float cosOuter = cos(light.spotOuterAngle);
    float cosInner = cos(light.spotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return baseLight * spotTerm;
}

#endif
#include "helpers.hlsli"
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage


cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    float padding;
    float4 ambientColor;
    Light lights[5];
}

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ShadowMap : register(t4);
SamplerState BasicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);



// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{ // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
// Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
// Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
        ShadowSampler,
        shadowUV,
        distToLight).r;
    
	//sample textures
    float2 uv = input.uv * uvScale + uvOffset;
    float4 surfaceColor = Albedo.Sample(BasicSampler, uv);
    surfaceColor = pow(surfaceColor, 2.2);
    float3 normalFromTexture = NormalMap.Sample(BasicSampler, uv).rgb;
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(0.04f, surfaceColor.rgb, metalness);
    
    
    //apply normal map
    float3 unpackedNormal = normalize((normalFromTexture * 2.0f) - 1.0f);
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    input.normal = normalize(mul(unpackedNormal, TBN));
    
    //float4 ambientTerm = ambientColor * surfaceColor * colorTint * (1 - metalness);
    
    float4 lightTerm = float4(0, 0, 0, 0);
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                lightTerm += DirectionalLight(input, lights[i], surfaceColor, cameraPosition, roughness, metalness, specularColor);
                break;
            case LIGHT_TYPE_POINT:
                lightTerm += PointLight(input, lights[i], surfaceColor, cameraPosition, roughness, metalness, specularColor);
                break;
            case LIGHT_TYPE_SPOT:
                lightTerm += SpotLight(input, lights[i], surfaceColor, cameraPosition, roughness, metalness, specularColor);
                break;

        }
        if (i == 0)
        {
            lightTerm *= shadowAmount;
        }
    }
    float4 finalColor = /*ambientTerm +*/ lightTerm;
    return pow(finalColor, 1.0 / 2.2);
}
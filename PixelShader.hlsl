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

Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);



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
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    float2 uv = input.uv * uvScale + uvOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv);
    
    input.normal = normalize(input.normal);
    
    
    float4 ambientTerm = ambientColor * surfaceColor * colorTint;
    
    float4 lightTerm = float4(0, 0, 0, 0);
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                lightTerm += DirectionalLight(input, lights[i], surfaceColor, cameraPosition);
                break;
            case LIGHT_TYPE_POINT:
                lightTerm += PointLight(input, lights[i], surfaceColor, cameraPosition);
                break;
            case LIGHT_TYPE_SPOT:
                lightTerm += SpotLight(input, lights[i], surfaceColor, cameraPosition);
                break;

        }
    }
    return ambientTerm + lightTerm;
}
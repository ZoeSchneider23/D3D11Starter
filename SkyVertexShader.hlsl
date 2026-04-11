#include "helpers.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

SkyVertexToPixel main(VertexShaderInput input)
{
    SkyVertexToPixel output;
    
    matrix modifiedView = view;
    modifiedView._14 = 0;
    modifiedView._24 = 0;
    modifiedView._34 = 0;
    output.position = mul(mul(projection, modifiedView), float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;
    
    return output;
}
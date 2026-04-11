#include "helpers.hlsli"

TextureCube skyTexture: register(t0);
SamplerState BasicSampler : register(s0);

float4 main(SkyVertexToPixel input) : SV_TARGET
{
    return skyTexture.Sample(BasicSampler, input.sampleDir);
}
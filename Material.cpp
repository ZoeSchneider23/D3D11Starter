#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader)
{
    this->colorTint = colorTint;
    this->vShader = vShader;
    this->pShader = pShader;
    uvOffset = DirectX::XMFLOAT2(0.0f, 0.0f);
    uvScale = DirectX::XMFLOAT2(1.0f, 1.0f);
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVShader()
{
    return vShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPShader()
{
    return pShader;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
    return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
    return uvOffset;
}

void Material::SetColorTint(DirectX::XMFLOAT4 value)
{
    colorTint = value;
}

void Material::SetVShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> value)
{
    vShader = value;
}

void Material::SetPShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> value)
{
    pShader = value;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 value)
{
    uvOffset = value;
}

void Material::SetUVScale(DirectX::XMFLOAT2 value)
{
    uvScale = value;
}

void Material::AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs[index] = srv;
}

void Material::AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> samp)
{
    samplers[index] = samp;
}

void Material::BindTexturesAndSamplers()
{
    //Shader Resources
    for (int i = 0; i < 128; i++) {
        Graphics::Context->PSSetShaderResources(i, 1, textureSRVs[i].GetAddressOf());
    }
    //Samplers
    for (int i = 0; i < 16; i++) {
        Graphics::Context->PSSetSamplers(i, 1, samplers[i].GetAddressOf());
    }
}

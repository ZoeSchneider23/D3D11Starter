#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader)
{
    this->colorTint = colorTint;
    this->vShader = vShader;
    this->pShader = pShader;
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

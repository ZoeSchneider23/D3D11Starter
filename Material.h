#pragma once
#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include <Windows.h>
#include "Graphics.h"

#include <wrl/client.h>
class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRVs[128];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[16];
public:
	Material(
		DirectX::XMFLOAT4 colorTint,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader
	);

	//Getters
	DirectX::XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPShader();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();
	//Setters
	void SetColorTint(DirectX::XMFLOAT4 value);
	void SetVShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> value);
	void SetPShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> value);
	void SetUVOffset(DirectX::XMFLOAT2 value);
	void SetUVScale(DirectX::XMFLOAT2 value);

	void AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> samp);
	void BindTexturesAndSamplers();
};


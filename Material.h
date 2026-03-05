#pragma once
#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include <Windows.h>

#include <wrl/client.h>
class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader;
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
	//Setters
	void SetColorTint(DirectX::XMFLOAT4 value);
	void SetVShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> value);
	void SetPShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> value);
};


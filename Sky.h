#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "Camera.h"

class Sky
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVertexShader;
	std::shared_ptr<Mesh> mesh;
public:
	Sky();
	Sky(std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPixelShader,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVertexShader, 
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
	~Sky();
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
	void Draw(Camera *cam);
};


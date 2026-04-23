#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include <vector>
#include "BufferStruct.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include <d3d11shadertracing.h>
#include <WICTextureLoader.h>
#include "Light.h"
#include "Sky.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateMaterials();
	void CreateGeometry();
	void LoadVertexShader(ID3DBlob* vertexShaderBlob);
	void LoadPixelShader(ID3DBlob* vertexShaderBlob);
	void FillAndBindNextConstantBuffer(
		void* data,
		unsigned int dataSizeInBytes,
		D3D11_SHADER_TYPE shaderType,
		unsigned int registerSlot);

	void ImguiUpdateHelper(float deltaTime);

	void InitializeLights();

	void BuildUI();


	//ImGui vars
	float bgColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool demoVisibility = true;
	float v_rad = 0;

	//Constant Buffer Struct
	VertexBufferStruct vsData;
	PixelBufferStruct psData;

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelConstantBuffer;

	// The D3D11.1 version of the Context object
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context1;
	// The one and only very large constant buffer (our GPU memory "heap")
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBufferHeap;
	// Size of the constant buffer heap (measured in bytes)
	unsigned int cbHeapSizeInBytes;
	// Position of the next unused portion of the heap
	unsigned int cbHeapOffsetInBytes;

	// Shaders and shader-related constructs
	std::vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
	std::vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::vector<std::shared_ptr<Material>> mats;

	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<DirectX::XMFLOAT3> entityTransformValues;
	std::vector<std::shared_ptr<Camera>> cams;
	int activeCam;

	std::vector<Light> lights;
	Sky sky;
	SkyVertexBufferStruct skyVsData;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;

	const int shadowMapResolution = 1024;
	float lightProjectionSize;
};


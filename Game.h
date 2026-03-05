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

	void ImguiUpdateHelper(float deltaTime);

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

	// Shaders and shader-related constructs
	std::vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
	std::vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::vector<std::shared_ptr<Material>> mats;

	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<DirectX::XMFLOAT3> entityTransformValues;
	std::vector<std::shared_ptr<Camera>> cams;
	int activeCam;
};


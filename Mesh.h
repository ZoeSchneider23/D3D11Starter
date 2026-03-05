#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <unordered_map>

class Mesh 
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int vertexCount;
	int indexCount;
	void CreateBuffers(Vertex* vertData, unsigned int vertNum, unsigned int* indexData, unsigned int indexNum);
public:
	Mesh(Vertex *vertData, unsigned int vertNum, unsigned int *indexData, unsigned int indexNum);
	Mesh(const char* objFile);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int getIndexCount();
	int getVertexCount();
	void Draw();
};

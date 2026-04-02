#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Light.h"

struct VertexBufferStruct {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 worldInvTranspose;
	float time;
};

struct PixelBufferStruct {
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT3 cameraPosition;
	float padding;
	DirectX::XMFLOAT4 ambientColor;
	Light lights[16];
};
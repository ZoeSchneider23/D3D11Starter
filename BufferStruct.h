#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct BufferStruct {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4X4 world;
};
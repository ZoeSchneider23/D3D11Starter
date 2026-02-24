#pragma once
#include "Input.h"
#include "Transform.h"
#include <memory>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	std::shared_ptr<Transform> transform;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	float fov;
	float nearClip = 0.01f;
	float farClip = 100;
	float movementSpeed = 5;
	float mouseLookSpeed = 0.003f;
public:
	Camera(XMFLOAT3 initialPosition, float aspectRatio, float fov);
	std::shared_ptr<Transform> GetTransform();
	XMFLOAT4X4 getViewMatrix();
	XMFLOAT4X4 getProjectionMatrix();
	float getFov();
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};


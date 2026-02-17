#include "Transform.h"


Transform::Transform()
{
    position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    scale = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&worldInverseTranspose, DirectX::XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
    this->position = position;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
    this->rotation = rotation;
}

void Transform::SetScale(float x, float y, float z)
{
    scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
    this->scale = scale;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
    return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
    return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
    return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX w = s * r * t;
    DirectX::XMStoreFloat4x4(&world, w);
    return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
    DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX w = s * r * t;
    DirectX::XMStoreFloat4x4(&worldInverseTranspose, 
        DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(w)));
    return worldInverseTranspose;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(position.x, position.y, position.z, 0);
    DirectX::XMVECTOR change = DirectX::XMVectorSet(x, y, z, 0);
    DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(pos, change));
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(position.x, position.y, position.z, 0);
    DirectX::XMVECTOR change = DirectX::XMLoadFloat3(&offset);
    DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(pos, change));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
    DirectX::XMVECTOR rot = DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 0);
    DirectX::XMVECTOR change = DirectX::XMVectorSet(pitch, yaw, roll, 0);
    DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorAdd(rot, change));
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
    DirectX::XMVECTOR rot = DirectX::XMVectorSet(this->rotation.x, this->rotation.y, this->rotation.z, 0);
    DirectX::XMVECTOR change = DirectX::XMLoadFloat3(&rotation);
    DirectX::XMStoreFloat3(&this->rotation, DirectX::XMVectorAdd(rot, change));
}

void Transform::Scale(float x, float y, float z)
{
    DirectX::XMVECTOR s = DirectX::XMVectorSet(scale.x, scale.y, scale.z, 0);
    DirectX::XMVECTOR change = DirectX::XMVectorSet(x,y,z, 0);
    DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorAdd(s, change));
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
    DirectX::XMVECTOR s = DirectX::XMVectorSet(this->scale.x, this->scale.y, this->scale.z, 0);
    DirectX::XMVECTOR change = DirectX::XMLoadFloat3(&rotation);
    DirectX::XMStoreFloat3(&this->rotation, DirectX::XMVectorMultiply(s, change));
}

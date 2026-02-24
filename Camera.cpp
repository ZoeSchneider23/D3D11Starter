#include "Camera.h"

Camera::Camera(XMFLOAT3 initialPosition, float aspectRatio, float fov)
{
    transform = std::make_shared<Transform>();
    transform->MoveAbsolute(initialPosition);
    this->fov = fov;
    UpdateProjectionMatrix(aspectRatio);
    UpdateViewMatrix();
}

std::shared_ptr<Transform> Camera::GetTransform()
{
    return transform;
}

XMFLOAT4X4 Camera::getViewMatrix()
{
    return viewMatrix;
}

XMFLOAT4X4 Camera::getProjectionMatrix()
{
    return projectionMatrix;
}

float Camera::getFov()
{
    return fov;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX mat = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
    XMStoreFloat4x4(&projectionMatrix, mat);
}

void Camera::UpdateViewMatrix()
{
    XMFLOAT3 pos = transform->GetPosition();
    XMFLOAT3 fwd = transform->GetForward();
    XMMATRIX mat = XMMatrixLookToLH(
        XMLoadFloat3(&pos),
        XMLoadFloat3(&fwd),
        XMVectorSet(0, 1, 0, 0)
    );
    XMStoreFloat4x4(&viewMatrix, mat);
}

void Camera::Update(float dt)
{
    if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, dt * movementSpeed); }
    if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -dt * movementSpeed); }
    if (Input::KeyDown('A')) { transform->MoveRelative(-dt * movementSpeed, 0, 0); }
    if (Input::KeyDown('D')) { transform->MoveRelative(dt * movementSpeed, 0, 0); }
    if (Input::KeyDown('X')) { transform->MoveAbsolute(0, -dt * movementSpeed, 0); }
    if (Input::KeyDown(VK_SPACE)) { transform->MoveAbsolute(0, dt * movementSpeed, 0); }
    //Mouse movement
    if (Input::MouseLeftDown())
    {
        float cursorMovementX = Input::GetMouseXDelta();
        float cursorMovementY = Input::GetMouseYDelta();
        float deltaX = cursorMovementX * mouseLookSpeed;
        float deltaY = cursorMovementY * mouseLookSpeed;
        transform->Rotate(deltaY, deltaX, 0);

        //Clamp rotation
        XMFLOAT3 rot = transform->GetPitchYawRoll();
        float totalPitch = rot.x;
        if (totalPitch > XM_PIDIV2) {
            totalPitch = XM_PIDIV2;
        }
        else if (totalPitch < -XM_PIDIV2) {
            totalPitch = -XM_PIDIV2;
        }
        transform->SetRotation(totalPitch, rot.y, rot.z);
    }
    UpdateViewMatrix();
}

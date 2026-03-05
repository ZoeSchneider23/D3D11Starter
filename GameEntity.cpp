#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat)
{
	transform = std::make_shared<Transform>();
	meshPointer = mesh;
	this->mat = mat;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return meshPointer;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Material> GameEntity::GetMat()
{
	return mat;
}

void GameEntity::SetMat(std::shared_ptr<Material> value)
{
	mat = value;
}

void GameEntity::Draw()
{
	Graphics::Context->VSSetShader(mat->GetVShader().Get(), 0, 0);
	Graphics::Context->PSSetShader(mat->GetPShader().Get(), 0, 0);
	meshPointer->Draw();
}

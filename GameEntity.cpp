#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	transform = std::make_shared<Transform>();
	meshPointer = mesh;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return meshPointer;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

void GameEntity::Draw()
{
	meshPointer->Draw();
}

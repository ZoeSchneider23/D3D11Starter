#include "GameEntity.h"

GameEntity::GameEntity(Mesh mesh)
{
	transform = Transform();
	meshPointer = std::make_shared<Mesh>(mesh);
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return meshPointer;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return std::shared_ptr<Transform>(&transform);
}

void GameEntity::Draw()
{
	meshPointer->Draw();
}

#pragma once
#include "Transform.h"
#include <memory>
#include "Mesh.h"
class GameEntity
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> meshPointer;
public:
	GameEntity(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw();
};


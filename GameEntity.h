#pragma once
#include "Transform.h"
#include <memory>
#include "Mesh.h"
#include "Material.h"
#include <d3d11.h>
class GameEntity
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> meshPointer;
	std::shared_ptr<Material> mat;
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMat();
	void SetMat(std::shared_ptr<Material> value);
	void Draw();
};


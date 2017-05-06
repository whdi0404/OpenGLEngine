#pragma once
#include "Transform.h"
class Camera;
class Octree;
class Material;
class Scene :
	public Transform
{
public:
	virtual void Initialize() {}
	virtual void Update() {}
	virtual void OnDrawGizmos() {}
};
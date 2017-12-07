#pragma once
#include "Component.h"

class TerrainSystem;
class Collider :
	public Component
{
public:
	Collider();
	virtual ~Collider() {}

	virtual void Update();

protected:
	PxMaterial* pxMaterial;
	PxRigidActor* pxRigidActor;
};
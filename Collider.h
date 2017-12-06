#pragma once
#include "Component.h"

class TerrainSystem;
class Rigidbody;
class Collider :
	public Component
{
public:
	Collider();
	virtual ~Collider() {}

	virtual void OnTriggerEnter(Collider* collider) = 0;
	virtual void OnTriggerExit(Collider* collider) = 0;

	virtual void Update();

	void SetStatic(bool isStatic);

protected:
	PxMaterial* pxMaterial;
	PxRigidActor* pxRigidActor;
};
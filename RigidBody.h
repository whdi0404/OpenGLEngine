#pragma once
#include "Collider.h"

class Camera;
class TerrainSystem;
class RigidBody : public Collider
{
private:
	PxGeometry* geometry;
	GetMacro(bool, IsStatic, isStatic);

public:
	enum
	{
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};
	RigidBody() {}
	~RigidBody() {}

	RigidBody* SetMesh(std::string meshKey, bool isStatic);

	virtual void Initialize() override;

	virtual void OnTriggerEnter(Collider * collider) override;
	virtual void OnTriggerExit(Collider * collider) override;
};
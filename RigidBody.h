#pragma once
#include "Collider.h"

class Camera;
class TerrainSystem;
class RigidBody : public Collider
{
	enum
	{
		CCD_FLAG = 1 << 29,
		DETACHABLE_FLAG = 1 << 31
	};
private:
	PxGeometry* geometry;
	GetMacro(bool, IsStatic, isStatic);

public:
	
	RigidBody() {}
	~RigidBody() {}

	RigidBody* SetGeometry(std::string meshKey, bool isStatic);
};
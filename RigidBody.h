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

	RigidBody* SetGeometry(PxGeometry* geometry, bool isStatic, glm::mat4x4 localPose = glm::mat4());
};
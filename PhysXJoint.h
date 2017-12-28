#pragma once
#include "Component.h"
#include "RigidBody.h"

class RigidBody;

class PhysXJoint : public Component
{
public:
	enum Type
	{
		RevoluteJoint,
		FixedJoint,
		DistanceJoint,
		SphericalJoint,
		D6Joint,
		PrismaticJoint
	};
	GetMacro(RigidBody*, LinkActor, linkActor[2]);

	GetMacro(PxJoint*, PxJoint, pxJoint);
	GetMacro(Type, Type, type);
public:
	PhysXJoint();
	~PhysXJoint();
	void LinkActor(Type type, RigidBody* rigidBodyA, PxTransform localFrameA, RigidBody* rigidBodyB, PxTransform localFrameB);
};
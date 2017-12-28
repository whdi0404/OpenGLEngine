#include "stdafx.h"
#include "PhysXJoint.h"

PhysXJoint::PhysXJoint()
{
}

PhysXJoint::~PhysXJoint()
{
}

void PhysXJoint::LinkActor(Type type, RigidBody * rigidBodyA, PxTransform localFrameA, RigidBody * rigidBodyB, PxTransform localFrameB)
{
	auto& physics = g_PhysXManager->GetPhysics();
	switch (type)
	{
	case Type::RevoluteJoint:
		pxJoint = PxRevoluteJointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	case Type::DistanceJoint:
		pxJoint = PxDistanceJointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	case Type::FixedJoint:
		pxJoint = PxFixedJointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	case Type::D6Joint:
		pxJoint = PxD6JointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	case Type::PrismaticJoint:
		pxJoint = PxPrismaticJointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	case Type::SphericalJoint:
		pxJoint = PxSphericalJointCreate(*physics, rigidBodyA->GetPxRigidActor(), localFrameA, rigidBodyB->GetPxRigidActor(), localFrameB);
		break;
	}
}
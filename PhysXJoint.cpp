#include "stdafx.h"
#include "PhysXJoint.h"

PhysXJoint::PhysXJoint()
{
}

PhysXJoint::~PhysXJoint()
{
}

void PhysXJoint::LinkActor(Type type, RigidBody * rigidBodyA, RigidBody * rigidBodyB, glm::mat4x4 localMatA, glm::mat4x4 localMatB)
{
	auto& physics = g_PhysXManager->GetPhysics();

	//auto pxLocalMatA = 

	auto pxLocalTransA = PxTransform(GetPxMatrixFromGLMMatrix(localMatA));
	auto pxLocalTransB = PxTransform(GetPxMatrixFromGLMMatrix(localMatB));

	switch (type)
	{
	case Type::RevoluteJoint:
		pxJoint = PxRevoluteJointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	case Type::DistanceJoint:
		pxJoint = PxDistanceJointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	case Type::FixedJoint:
		pxJoint = PxFixedJointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	case Type::D6Joint:
		pxJoint = PxD6JointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	case Type::PrismaticJoint:
		pxJoint = PxPrismaticJointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	case Type::SphericalJoint:
		pxJoint = PxSphericalJointCreate(*physics, rigidBodyA->GetPxRigidActor(), pxLocalTransA, rigidBodyB->GetPxRigidActor(), pxLocalTransB);
		break;
	}

	/*switch (type)
	{
	case Type::RevoluteJoint:
		pxJoint = PxRevoluteJointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	case Type::DistanceJoint:
		pxJoint = PxDistanceJointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	case Type::FixedJoint:
		pxJoint = PxFixedJointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	case Type::D6Joint:
		pxJoint = PxD6JointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	case Type::PrismaticJoint:
		pxJoint = PxPrismaticJointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	case Type::SphericalJoint:
		pxJoint = PxSphericalJointCreate(*physics, rigidBodyA->GetPxRigidActor(), rigidbodyAInvPose, rigidBodyB->GetPxRigidActor(), rigidbodyBInvPose);
		break;
	}*/
}
#include "stdafx.h"
#include "RigidBody.h"
#include "Gizmo.h"
#include "PhysXResourceManager.h"

RigidBody * RigidBody::SetMesh(std::string meshKey, bool isStatic)
{
	this->isStatic = isStatic;
	auto& resourceManager = g_PhysXManager->GetResources();
	geometry = resourceManager->GetResource<PxGeometry>(meshKey);

	pxMaterial = g_PhysXManager->GetResources()->GetResource<PxMaterial>("default_Material");

	if (isStatic)
	{
		PxRigidStatic* pxRigidStatic = g_PhysXManager->GetPhysics()->createRigidStatic(PxTransform(GetPxVec3FromGLMVec3(GetTransform()->GetWorldPosition())));
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*pxRigidStatic, *geometry, *pxMaterial);

		pxRigidActor = pxRigidStatic;
	}
	else
	{
		PxRigidDynamic* pxRigidDynamic = g_PhysXManager->GetPhysics()->createRigidDynamic(PxTransform(GetPxVec3FromGLMVec3(GetTransform()->GetWorldPosition())));
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*pxRigidDynamic, *geometry, *pxMaterial);

		pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		PxFilterData fd = shape->getSimulationFilterData();
		fd.word3 |= CCD_FLAG;
		shape->setSimulationFilterData(fd);
		PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, 1);

		pxRigidActor = pxRigidDynamic;
	}

	g_PhysXManager->GetScene()->addActor(*pxRigidActor);

	return this;
}

void RigidBody::Initialize()
{
}

void RigidBody::OnTriggerEnter(Collider * collider)
{
}

void RigidBody::OnTriggerExit(Collider * collider)
{
}
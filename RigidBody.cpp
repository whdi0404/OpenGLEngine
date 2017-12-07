#include "stdafx.h"
#include "RigidBody.h"
#include "Gizmo.h"
#include "ResourceManager.h"

RigidBody * RigidBody::SetGeometry(std::string geometryKey, bool isStatic)
{
	this->isStatic = isStatic;
	geometry = ResourceManager::GetInstance().GetResource<PxGeometry>(geometryKey);

	pxMaterial = ResourceManager::GetInstance().GetResource<PxMaterial>("default_Material");

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

#include "stdafx.h"
#include "RigidBody.h"
#include "Gizmo.h"
#include "ResourceManager.h"

RigidBody * RigidBody::SetGeometry(PxGeometry* geom, bool isStatic)
{
	this->isStatic = isStatic;
	geometry = geom;

	pxMaterial = ResourceManager::GetInstance().GetResource<PxMaterial>("default_Material");

	PxVec3 pxPos = GetPxVec3FromGLMVec3(GetTransform()->GetWorldPosition());
	
	glm::quat rot = GetTransform()->GetWorldQuaternion();
	PxQuat pxRot = PxQuat(rot.x, rot.y, rot.z, rot.w);

	if (isStatic)
	{
		PxRigidStatic* pxRigidStatic = g_PhysXManager->GetPhysics()->createRigidStatic(PxTransform(pxPos, pxRot));
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*pxRigidStatic, *geometry, *pxMaterial);

		pxRigidActor = pxRigidStatic;
	}
	else
	{
		PxRigidDynamic* pxRigidDynamic = g_PhysXManager->GetPhysics()->createRigidDynamic(PxTransform(pxPos, pxRot));
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

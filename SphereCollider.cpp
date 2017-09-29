#include "stdafx.h"
#include "SphereCollider.h"
#include "Gizmo.h"

void SphereCollider::Initialize()
{
	PxRigidDynamic* pxRigidDynamic = g_PhysXManager->GetPhysics()->createRigidDynamic(PxTransform(GetPxVec3FromGLMVec3(GetTransform()->GetWorldPosition())));
	//юс╫ц
	pxMaterial = g_PhysXManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

	PxShape* ballShape = PxRigidActorExt::createExclusiveShape(*pxRigidDynamic, PxSphereGeometry(1.0f), *pxMaterial);
	{
		PxFilterData fd = ballShape->getSimulationFilterData();
		fd.word3 |= SNOWBALL_FLAG;
		ballShape->setSimulationFilterData(fd);
	}
	//CCD Use
	{
		pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		PxFilterData fd = ballShape->getSimulationFilterData();
		fd.word3 |= CCD_FLAG;
		ballShape->setSimulationFilterData(fd);
	}
	PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, 1);

	g_PhysXManager->GetScene()->addActor(*pxRigidDynamic);

	pxRigidActor = pxRigidDynamic;
}

void SphereCollider::OnTriggerEnter(Collider * collider)
{
}

void SphereCollider::OnTriggerExit(Collider * collider)
{
}

void SphereCollider::OnDrawGizmo()
{
	Gizmo::DrawSphere(camera, GetTransform()->GetWorldPosition(), 1, 4);
}
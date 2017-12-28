#include "stdafx.h"
#include "Collider.h"
#include "TerrainSystem.h"


Collider::Collider() : pxRigidActor(nullptr), pxMaterial(nullptr)
{
}

void Collider::Update()
{
	PxSceneReadLock scopedLock(*g_PhysXManager->GetScene());
	PxTransform transform = pxRigidActor->getGlobalPose();

	GetTransform()->SetWorldPosition(GetGLMVec3FromPxVec3(transform.p));
	GetTransform()->SetRotateWorld(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
}
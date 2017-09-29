#include "stdafx.h"
#include "TerrainCollider.h"
#include "TerrainSystem.h"
#include "GameObject.h"

void TerrainCollider::OnTriggerEnter(Collider * collider)
{
}

void TerrainCollider::OnTriggerExit(Collider * collider)
{
}

void TerrainCollider::Initialize()
{
	terrain = GetComponent<TerrainSystem>();
	CreateCollider();
}

void TerrainCollider::CreateCollider()
{
	//юс╫ц
	pxMaterial = g_PhysXManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

	const PxReal heightScale = terrain->GetMaxHeight() * (1.0f/65535);

	glm::ivec2 heightmapSize = terrain->GetTerrainSize();

	PxU32 hfNumVerts = heightmapSize.x*heightmapSize.y;

	PxHeightFieldSample* samples = (PxHeightFieldSample*)SAMPLE_ALLOC(sizeof(PxHeightFieldSample)*hfNumVerts);
	memset(samples, 0, hfNumVerts * sizeof(PxHeightFieldSample));

 	for (PxU32 x = 0; x < heightmapSize.x; x++)
		for (PxU32 y = 0; y < heightmapSize.y; y++)
		{
			PxI32 h = PxI32(terrain->GetHeight(glm::ivec2(x, y)));
			PX_ASSERT(h <= 0xffff);
			samples[x + y*heightmapSize.x].height = (PxI16)(h);
			samples[x + y*heightmapSize.x].setTessFlag();
			samples[x + y*heightmapSize.x].materialIndex0 = 1;
			samples[x + y*heightmapSize.x].materialIndex1 = 1;
		}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = heightmapSize.x;
	hfDesc.nbRows = heightmapSize.y;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* heightField = g_PhysXManager->GetCookie()->createHeightField(hfDesc, g_PhysXManager->GetPhysics()->getPhysicsInsertionCallback());

	PxTransform pose = PxTransform(PxIdentity);
	/*pose.p = mm
	mHFPose = pose;
	*/
	PxRigidStatic* hfActor = g_PhysXManager->GetPhysics()->createRigidStatic(pose);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), 1, 1.0f, 1.0f);
	PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *pxMaterial);

	g_PhysXManager->GetScene()->addActor(*hfActor);

	SAMPLE_FREE(samples);
}
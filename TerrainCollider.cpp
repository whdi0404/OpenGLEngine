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

	PxReal heightScale = terrain->GetHeightScale() / 50;

	glm::ivec2 heightmapSize = terrain->GetTerrainSize();
	heightmapSize.x += 1;
	heightmapSize.y += 1;

	PxU32 hfNumVerts = (heightmapSize.x) * (heightmapSize.y);

	PxHeightFieldSample* samples = (PxHeightFieldSample*)SAMPLE_ALLOC(sizeof(PxHeightFieldSample)*hfNumVerts);
	memset(samples, 0, hfNumVerts * sizeof(PxHeightFieldSample));

 	for (int x = 0; x < heightmapSize.x; x++)
		for (int y = 0; y < heightmapSize.y; y++)
		{
			PxI32 h = PxI32(terrain->GetHeight(glm::ivec2(glm::min(y, terrain->GetTerrainSize().y - 1), glm::min(x, terrain->GetTerrainSize().x - 1))));
			PX_ASSERT(h <= 0xffff);
			samples[x + y * heightmapSize.x].height = (PxI16)(h);
			samples[x + y * heightmapSize.x].setTessFlag();
			samples[x + y * heightmapSize.x].materialIndex0 = 1;
			samples[x + y * heightmapSize.x].materialIndex1 = 1;
		}

	PxHeightFieldDesc hfDesc;
	hfDesc.setToDefault();
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = heightmapSize.x;
	hfDesc.nbRows = heightmapSize.y;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* heightField = g_PhysXManager->GetCookie()->createHeightField(hfDesc, g_PhysXManager->GetPhysics()->getPhysicsInsertionCallback());
	
	auto& tileSize = terrain->GetTileSize();
	
	PxTransform pose = PxTransform(PxIdentity);

	PxRigidStatic* hfActor = g_PhysXManager->GetPhysics()->createRigidStatic(pose);

	float xSize = tileSize * ((float)(heightmapSize.x) / (float)(heightmapSize.x + 1));
	float ySize = tileSize * ((float)(heightmapSize.y) / (float)(heightmapSize.y + 1));
	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, xSize, ySize);
	PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *pxMaterial);

	g_PhysXManager->GetScene()->addActor(*hfActor);

	SAMPLE_FREE(samples);
}
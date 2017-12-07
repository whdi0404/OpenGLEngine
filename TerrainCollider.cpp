#include "stdafx.h"
#include "TerrainCollider.h"
#include "TerrainSystem.h"
#include "GameObject.h"
#include "Camera.h"
#include "ResourceManager.h"

void TerrainCollider::Initialize()
{
	terrain = GetComponent<TerrainSystem>();
	tessellationFactor = 2;
	clusterCellCount = 128;
	clusterScale = terrain->GetTileScale() / tessellationFactor * clusterCellCount;

	CreateCollider();
}

void TerrainCollider::Update()
{
	for (auto& heightCollider : heightColliders)
	{
		auto pos = heightCollider->getGlobalPose().p;
		auto camPos = Camera::GetMainCamera()->GetTransform()->GetWorldPosition();

		float len = glm::length(glm::vec2(camPos.x, camPos.z) - (glm::vec2(pos.x, pos.z) + clusterScale * 0.5f));

		if (len < 64)
		{
			if(heightCollider->getScene() == nullptr)
				g_PhysXManager->GetScene()->addActor(*heightCollider);
		}
		else if (heightCollider->getScene() != nullptr)
		{
			g_PhysXManager->GetScene()->removeActor(*heightCollider);
		}
	}
}

void TerrainCollider::CreateCollider()
{
	pxMaterial = ResourceManager::GetInstance().GetResource<PxMaterial>("default_Material");

	float precision = UINT16_MAX * 0.49f / terrain->GetHeightScale();
	PxReal heightScale = 1.0f / precision;
	float tileSize = terrain->GetTileScale() / tessellationFactor;

	glm::ivec2 heightmapSize = terrain->GetTerrainSize() * tessellationFactor;
	heightmapSize += tessellationFactor;

	int xClusterNumber = heightmapSize.x / clusterCellCount;
	int yClusterNumber = heightmapSize.y / clusterCellCount;

	for (int i = 0; i < xClusterNumber; ++i)
	{
		for (int j = 0; j < yClusterNumber; ++j)
		{
			int width = clusterCellCount + 1;
			int height = clusterCellCount + 1;
			int xStart = i * clusterCellCount;
			int yStart = j * clusterCellCount;

			PxHeightFieldSample* samples = (PxHeightFieldSample*)SAMPLE_ALLOC(sizeof(PxHeightFieldSample) * width * height);
			memset(samples, 0, width * height * sizeof(PxHeightFieldSample));

			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
				{
					glm::vec2 uv = glm::vec2(((float)(yStart + y) / heightmapSize.y), ((float)(xStart + x) / heightmapSize.x));
					PxI32 h = PxI32(terrain->GetHeight(uv) * precision);
					PX_ASSERT(h <= 0xffff);
					samples[x + y * width].height = (PxI16)(h);
					samples[x + y * width].setTessFlag();
					samples[x + y * width].materialIndex0 = 1;
					samples[x + y * width].materialIndex1 = 1;
				}

			PxHeightFieldDesc hfDesc;
			hfDesc.setToDefault();
			hfDesc.format = PxHeightFieldFormat::eS16_TM;
			hfDesc.nbColumns = width;
			hfDesc.nbRows = height;
			hfDesc.samples.data = samples;
			hfDesc.samples.stride = sizeof(PxHeightFieldSample);
			//hfDesc.convexEdgeThreshold = 1.0f;

			PxHeightField* heightField = g_PhysXManager->GetCookie()->createHeightField(hfDesc, g_PhysXManager->GetPhysics()->getPhysicsInsertionCallback());

			PxTransform trans(PxIdentity);
			trans.p = PxVec3(yStart * tileSize, 0, xStart * tileSize);
			PxRigidStatic* hfActor = g_PhysXManager->GetPhysics()->createRigidStatic(trans);

			PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, tileSize, tileSize);
			PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *pxMaterial);
			heightColliders.push_back(hfActor);

			SAMPLE_FREE(samples);
		}
	}
}
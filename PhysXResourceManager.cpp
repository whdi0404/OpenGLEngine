#include "stdafx.h"
#include "PhysXResourceManager.h"
#include "Mesh.h"
#include "VertexBuffer.h"

PhysXResourceManager::PhysXResourceManager()
{
}


PhysXResourceManager::~PhysXResourceManager()
{
}

void PhysXResourceManager::AddGeometryResource(std::string key, Mesh* renderMesh, PxVec3 sc, PxQuat rot)
{
	PxPhysics& physics = *g_PhysXManager->GetPhysics();
	PxCooking& cooking = *g_PhysXManager->GetCookie();

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = renderMesh->GetVertexBuffer()->GetVertexCount();
	meshDesc.triangles.count = renderMesh->GetIndices().size() / 3;
	meshDesc.points.stride = sizeof(glm::vec3);
	meshDesc.triangles.stride = sizeof(int) * 3;
	auto& vertices = renderMesh->GetVertexBuffer()->GetVectorList<glm::vec3>(Element::Position);
	meshDesc.points.data = vertices.data();
	meshDesc.triangles.data = renderMesh->GetIndices().data();

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = cooking.cookTriangleMesh(meshDesc, writeBuffer, &result);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	// disable mesh cleaning - perform mesh validation on development configurations
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// disable edge precompute, edges are set for each triangle, slows contact generation
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	// lower hierarchy for internal mesh
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	cooking.setParams(params);

#ifdef _DEBUG
	// mesh should be validated before cooked without the mesh cleaning
	bool res = theCooking->validateTriangleMesh(meshDesc);
	PX_ASSERT(res);
#endif
	PxTriangleMesh* mesh = cooking.createTriangleMesh(meshDesc, physics.getPhysicsInsertionCallback());
	PX_ASSERT(mesh);

	PxMeshScale meshScale = PxMeshScale(sc, rot);
	PxGeometry* geometry = new PxTriangleMeshGeometry(mesh, meshScale);

	AddResource(key, geometry);
}

void PhysXResourceManager::AddSphereGeomResource(std::string key, float radius)
{
	PxGeometry* geometry = new PxSphereGeometry(radius);

	AddResource(key, geometry);
}

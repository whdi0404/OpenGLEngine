#include "stdafx.h"
#include "Mesh.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "GameObject.h"
#include "PhysXUtil.h"
#include "RigidBody.h"
#include "ResourceManager.h"
#include "PhysXJoint.h"
#include "SkinnedMeshRenderObject.h"

PxGeometry* PhysXUtil::MeshToPxGeometry(Mesh* renderMesh, PxVec3 sc, PxQuat rot)
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

	return geometry;
}

PxGeometry * PhysXUtil::MeshToPxConvexGeometry(Mesh * renderMesh, PxVec3 sc, PxQuat rot)
{
	auto& cooking = g_PhysXManager->GetCookie();
	auto& physics = g_PhysXManager->GetPhysics();

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = renderMesh->GetVertexBuffer()->GetVertexCount();
	convexDesc.points.stride = sizeof(glm::vec3);
	convexDesc.points.data = renderMesh->GetVertexBuffer()->GetVectorList<glm::vec3>(Element::Position).data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	//convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		return NULL;
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = physics->createConvexMesh(input);

	PxMeshScale meshScale = PxMeshScale(sc, rot);
	PxGeometry* geometry = new PxConvexMeshGeometry(convexMesh, meshScale);

	return geometry;
}


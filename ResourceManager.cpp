#include "stdafx.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "PhysXUtil.h"

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

void ResourceManager::AddPxTriangleMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 sc, PxQuat rot)
{
	PxGeometry* geometry = PhysXUtil::MeshToPxGeometry(renderMesh, sc, rot);
	AddResource(key, geometry);
}

void ResourceManager::AddPxConvexMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 sc, PxQuat rot)
{
	PxGeometry* geometry = PhysXUtil::MeshToPxConvexGeometry(renderMesh, sc, rot);
	AddResource(key, geometry);
}

void ResourceManager::AddPxSphereGeometryResource(std::string key, float radius)
{
	PxGeometry* geometry = new PxSphereGeometry(radius);
	AddResource(key, geometry);
}

void ResourceManager::AddPxCapsuleGeometryResource(std::string key, float radius, float height)
{
	PxGeometry* geometry = new PxCapsuleGeometry(radius, height);
	AddResource(key, geometry);
}

void ResourceManager::AddPxBoxGeometryResource(std::string key, glm::vec3 boxHalfSize)
{
	PxGeometry* geometry = new PxBoxGeometry(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z);
	AddResource(key, geometry);
}

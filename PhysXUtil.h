#pragma once
static class PhysXUtil
{
private:
	PhysXUtil() {}

public:
	static PxGeometry* MeshToPxGeometry(Mesh* renderMesh, PxVec3 sc = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	static PxGeometry* MeshToPxConvexGeometry(Mesh* renderMesh, PxVec3 sc = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
};
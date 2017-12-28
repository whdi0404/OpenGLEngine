#pragma once

class Transform;
class SkinnedMeshRenderObject;
static class PhysXUtil
{
public:
	struct RagdollInfo
	{
	public:
		//13 Bones
		Transform* Pelvis;
		Transform* LeftHips;
		Transform* LeftKnee;
		Transform* LeftFoot;
		Transform* LeftArm;
		Transform* LeftElbow;
		Transform* RightHips;
		Transform* RightKnee;
		Transform* RightFoot;
		Transform* RightArm;
		Transform* RightElbow;
		Transform* MiddleSpine;
		Transform* Head;
		GameObject* RootObject;
	};

private:
	PhysXUtil() {}
	
public:
	static PxGeometry* MeshToPxGeometry(Mesh* renderMesh, PxVec3 sc = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	static PxGeometry* MeshToPxConvexGeometry(Mesh* renderMesh, PxVec3 sc = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	static void MakeRagdoll(std::string id, RagdollInfo ragdollInfo);
};
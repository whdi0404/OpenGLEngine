#pragma once
#include "Component.h"
class SkinnedMesh;
class RigidBody;
class Avatar;
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

class Ragdoll :
	public Component
{
private:
	struct RagdollBone
	{
		int boneIndex;
		RigidBody* rigidBody;
		glm::mat4x4 deformMat;
	};

private:
	RagdollBone rootBone;
	std::vector<RagdollBone> ragdollBones;

public:
	Ragdoll();
	~Ragdoll();
	GetSetMacro(Avatar*, Avatar, avatar);

	void Update() override;

	void MakeRagdoll(std::string id, RagdollInfo ragdollInfo);
};
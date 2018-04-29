#include "stdafx.h"
#include "Ragdoll.h"
#include "PhysXJoint.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Avatar.h"
#include "Time.h"

Ragdoll::Ragdoll()
{
}


Ragdoll::~Ragdoll()
{
}

void Ragdoll::Update()
{
	GetTransform()->SetWorldMatrix(rootBone.rigidBody->GetTransform()->GetWorldMatrix() * rootBone.deformMat);
	
	for (int i = 0; i < ragdollBones.size(); ++i)
	{
		//deform = inverse(리지드바디) * 본
		//본 = 리지드바디 * deform * inverse(world??)
		avatar->GetBoneTrans()[ragdollBones[i].boneIndex]->SetWorldMatrix(glm::inverse(GetTransform()->GetWorldMatrix()) * ragdollBones[i].rigidBody->GetTransform()->GetWorldMatrix() * ragdollBones[i].deformMat);
	}
	avatar->Update();
}

void Ragdoll::MakeRagdoll(std::string id, RagdollInfo ragdollInfo)
{
	avatar->GetRoot()->SetLocalScale(GetTransform()->GetLocalScale());
	PxSceneWriteLock lock(*g_PhysXManager->GetScene());

	std::unordered_map<std::string, PxGeometry*>* resources = ResourceManager::GetInstance().GetResource<std::unordered_map<std::string, PxGeometry*>>(id);

	glm::vec3 middleSpinePos = ragdollInfo.MiddleSpine->GetWorldPosition();
	glm::vec3 leftHipsPos = ragdollInfo.LeftHips->GetWorldPosition();
	glm::vec3 rightHipsPos = ragdollInfo.RightHips->GetWorldPosition();
	glm::vec3 leftKneePos = ragdollInfo.LeftKnee->GetWorldPosition();
	glm::vec3 rightKneePos = ragdollInfo.RightKnee->GetWorldPosition();
	glm::vec3 leftFootPos = ragdollInfo.LeftFoot->GetWorldPosition();
	glm::vec3 rightFootPos = ragdollInfo.RightFoot->GetWorldPosition();
	glm::vec3 leftArmPos = ragdollInfo.LeftArm->GetWorldPosition();
	glm::vec3 rightArmPos = ragdollInfo.RightArm->GetWorldPosition();
	glm::vec3 leftElbowPos = ragdollInfo.LeftElbow->GetWorldPosition();
	glm::vec3 rightElbowPos = ragdollInfo.RightElbow->GetWorldPosition();
	glm::vec3 headPos = ragdollInfo.Head->GetWorldPosition();
	glm::vec3 pelvisPos = ragdollInfo.Pelvis->GetWorldPosition();

	float pelvisTop = +abs(pelvisPos.y - middleSpinePos.y);
	float pelvisBottom = -abs(pelvisPos.y - leftHipsPos.y);
	float pelvisLeft = -abs(pelvisPos.x - leftHipsPos.x);
	float pelvisRight = +abs(pelvisPos.x - rightHipsPos.x);
	float margin = (pelvisRight - pelvisLeft) * 0.5f * 0.5f;

	float leftLegToKnee = glm::length(leftHipsPos - leftKneePos) - (margin * 1.5f);
	float rightLegToKnee = glm::length(rightHipsPos - rightKneePos) - (margin * 1.5f);
	float leftKneeToFoot = glm::length(leftKneePos - leftFootPos) - (margin * 1.5f);
	float rightKneeToFoot = glm::length(rightKneePos - rightFootPos) - (margin * 1.5f);

	float midSpineToLeftArm = glm::length(leftArmPos - middleSpinePos) - (margin * 1.5f);
	float midSpineToRightArm = glm::length(rightArmPos - middleSpinePos) - (margin * 1.5f);

	float leftArmToElbow = glm::length(leftElbowPos - leftArmPos) - (margin * 1.5f);
	float rightArmToElbow = glm::length(rightElbowPos - rightArmPos) - (margin * 1.5f);

	float midSpineToHead = leftArmPos.y - middleSpinePos.y;

	float upperBodyToHead = headPos.y - leftArmPos.y;

	if (resources == nullptr)
	{
		resources = new std::unordered_map<std::string, PxGeometry*>();

		PxBoxGeometry* pelvisGeom = new PxBoxGeometry(PxVec3((pelvisRight - pelvisLeft) * 0.85f, (pelvisTop - pelvisBottom) * 0.4f, (pelvisRight - pelvisLeft) * 0.4f));
		resources->insert(std::make_pair("pelvis", pelvisGeom));

		PxBoxGeometry* upperBodyGeom = new PxBoxGeometry(PxVec3(pelvisGeom->halfExtents.x, midSpineToHead / 2, pelvisGeom->halfExtents.z));
		resources->insert(std::make_pair("upperBody", upperBodyGeom));

		PxCapsuleGeometry* leftLegGeom = new PxCapsuleGeometry(margin * 1.5f, leftLegToKnee / 2);
		resources->insert(std::make_pair("leftHips", leftLegGeom));

		PxCapsuleGeometry* rightLegGeom = new PxCapsuleGeometry(margin * 1.5f, rightLegToKnee / 2);
		resources->insert(std::make_pair("rightHips", rightLegGeom));

		PxCapsuleGeometry* leftKneeGeom = new PxCapsuleGeometry(margin * 1.5f, leftKneeToFoot / 2);
		resources->insert(std::make_pair("leftKnee", leftKneeGeom));

		PxCapsuleGeometry* rightKneeGeom = new PxCapsuleGeometry(margin * 1.5f, rightKneeToFoot / 2);
		resources->insert(std::make_pair("rightKnee", rightKneeGeom));

		PxCapsuleGeometry* leftArmGeom = new PxCapsuleGeometry(margin * 1.5f, midSpineToLeftArm / 2);
		resources->insert(std::make_pair("leftArm", leftArmGeom));

		PxCapsuleGeometry* rightArmGeom = new PxCapsuleGeometry(margin * 1.5f, midSpineToRightArm / 2);
		resources->insert(std::make_pair("rightArm", rightArmGeom));

		PxCapsuleGeometry* leftElbowGeom = new PxCapsuleGeometry(margin * 1.5f, leftArmToElbow / 2);
		resources->insert(std::make_pair("leftElbow", leftElbowGeom));

		PxCapsuleGeometry* rightElbowGeom = new PxCapsuleGeometry(margin * 1.5f, rightArmToElbow / 2);
		resources->insert(std::make_pair("rightElbow", rightElbowGeom));

		PxCapsuleGeometry* headGeom = new PxCapsuleGeometry(margin * 2, upperBodyToHead / 2);
		resources->insert(std::make_pair("head", headGeom));

		//ResourceManager::GetInstance().AddResource(id, resources);
	}
	//몸통
	GameObject* pelvisGO = (new GameObject("pelvis"))->GetTransform()->SetWorldPosition(pelvisPos).GetGameObject();
	RigidBody* pelvisRigidBody = pelvisGO->AddComponent<RigidBody>()->SetGeometry((*resources)["pelvis"], false, glm::translate(glm::vec3(0, (pelvisTop - pelvisBottom) * 0.4f, 0)));

	GameObject* upperBodyGO = (new GameObject("upperBody"))->GetTransform()->SetWorldPosition(middleSpinePos).GetGameObject();
	RigidBody* upperBodyRigidBody = upperBodyGO->AddComponent<RigidBody>()->SetGeometry((*resources)["upperBody"], false, glm::translate(glm::vec3(0, midSpineToHead / 2, 0)));

	PhysXJoint* pelvisToUpperBodyJoint = pelvisRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	pelvisToUpperBodyJoint->LinkActor(PhysXJoint::Type::D6Joint, pelvisRigidBody, upperBodyRigidBody,
		glm::translate(upperBodyGO->GetTransform()->GetWorldPosition() - pelvisGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxPelvisToUBjoint = (PxD6Joint*)pelvisToUpperBodyJoint->GetPxJoint();
	pxPelvisToUBjoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-10.0f), glm::radians(15.0f)));
	pxPelvisToUBjoint->setSwingLimit(PxJointLimitCone(glm::radians(10.0f), 0, 0.1f));
	pxPelvisToUBjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxPelvisToUBjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	pxPelvisToUBjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);

	//머리
	glm::vec3 headWorldPos = headPos;
	headWorldPos.y = leftArmPos.y + margin * 2;
	GameObject* headGO = (new GameObject("head"))->GetTransform()->SetWorldPosition(headWorldPos).GetGameObject();
	RigidBody* headRigidBody = headGO->AddComponent<RigidBody>()->SetGeometry((*resources)["head"], false, glm::translate(glm::vec3(0, upperBodyToHead / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(-90.0f)));

	PhysXJoint* UpperBodyToHeadJoint = pelvisRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	UpperBodyToHeadJoint->LinkActor(PhysXJoint::Type::D6Joint, upperBodyRigidBody, headRigidBody,
		glm::translate(headGO->GetTransform()->GetWorldPosition() - upperBodyGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxUBToHeadjoint = (PxD6Joint*)UpperBodyToHeadJoint->GetPxJoint();
	pxUBToHeadjoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-30.0f), glm::radians(30.0f)));
	pxUBToHeadjoint->setSwingLimit(PxJointLimitCone(glm::radians(45.0f), glm::radians(20.0f), 0.1f));
	pxUBToHeadjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxUBToHeadjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	pxUBToHeadjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

	//다리
	GameObject* leftHipsGO = (new GameObject("leftHips"))->GetTransform()->SetWorldPosition(leftHipsPos).GetGameObject();
	RigidBody* leftHipsRigidBody = leftHipsGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftHips"], false,
		glm::translate(glm::vec3(0, -leftLegToKnee / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* rightHipsGO = (new GameObject("rightHips"))->GetTransform()->SetWorldPosition(rightHipsPos).GetGameObject();
	RigidBody* rightHipsRigidBody = rightHipsGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightHips"], false,
		glm::translate(glm::vec3(0, -rightLegToKnee / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* leftKneeGO = (new GameObject("leftKnee"))->GetTransform()->SetWorldPosition(leftKneePos + glm::vec3(0, -margin * 1.5f * 0.5f, 0)).GetGameObject();
	RigidBody* leftKneeRigidBody = leftKneeGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftKnee"], false,
		glm::translate(glm::vec3(0, -leftKneeToFoot / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* rightKneeGO = (new GameObject("rightKnee"))->GetTransform()->SetWorldPosition(rightKneePos + glm::vec3(0, -margin * 1.5f * 0.5f, 0)).GetGameObject();
	RigidBody* rightKneeRigidBody = rightKneeGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightKnee"], false,
		glm::translate(glm::vec3(0, -rightKneeToFoot / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	PhysXJoint* pevisToLeftHipsJoint = pelvisRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	pevisToLeftHipsJoint->LinkActor(PhysXJoint::Type::D6Joint, pelvisRigidBody, leftHipsRigidBody,
		glm::translate(leftHipsGO->GetTransform()->GetWorldPosition() - pelvisGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxPelvisToLHjoint = (PxD6Joint*)pevisToLeftHipsJoint->GetPxJoint();
	pxPelvisToLHjoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-70.0f), glm::radians(20.0f)));
	pxPelvisToLHjoint->setSwingLimit(PxJointLimitCone(glm::radians(90.0f), glm::radians(30.0f), 0.1f));
	pxPelvisToLHjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxPelvisToLHjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxPelvisToLHjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

	PhysXJoint* pevisToRightHipsJoint = pelvisRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	pevisToRightHipsJoint->LinkActor(PhysXJoint::Type::D6Joint, pelvisRigidBody, rightHipsRigidBody,
		glm::translate(rightHipsGO->GetTransform()->GetWorldPosition() - pelvisGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxPelvisToRHjoint = (PxD6Joint*)pevisToRightHipsJoint->GetPxJoint();
	pxPelvisToRHjoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-70.0f), glm::radians(20.0f)));
	pxPelvisToRHjoint->setSwingLimit(PxJointLimitCone(glm::radians(90.0f), glm::radians(30.0f), 0.1f));
	pxPelvisToRHjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxPelvisToRHjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxPelvisToRHjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

	PhysXJoint* leftKneeToFootJoint = leftHipsRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	leftKneeToFootJoint->LinkActor(PhysXJoint::Type::D6Joint, leftHipsRigidBody, leftKneeRigidBody,
		glm::translate(leftKneeGO->GetTransform()->GetWorldPosition() - leftHipsGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxLKToFootJoint = (PxD6Joint*)leftKneeToFootJoint->GetPxJoint();
	pxLKToFootJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(0.0f), glm::radians(80.0f)));
	pxLKToFootJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxLKToFootJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxLKToFootJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);

	PhysXJoint* rightKneeToFootJoint = rightHipsRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	rightKneeToFootJoint->LinkActor(PhysXJoint::Type::D6Joint, rightHipsRigidBody, rightKneeRigidBody,
		glm::translate(rightKneeGO->GetTransform()->GetWorldPosition() - rightHipsGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxRKToFootJoint = (PxD6Joint*)rightKneeToFootJoint->GetPxJoint();
	pxRKToFootJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(0.0f), glm::radians(80.0f)));
	pxRKToFootJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxRKToFootJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxRKToFootJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);

	//팔
	GameObject* leftArmGO = (new GameObject("leftArm"))->GetTransform()->SetWorldPosition(leftArmPos + glm::vec3(margin * 2.0f, 0, 0)).GetGameObject();
	RigidBody* leftArmRigidBody = leftArmGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftArm"], false,
		glm::translate(glm::vec3(midSpineToLeftArm / 2, 0, 0)));

	glm::mat4x4 leftElbowRot = glm::axisAngleMatrix(glm::vec3(0, 1, 0), glm::radians(90.0f));
	GameObject* leftElbowGO = (new GameObject("leftElbow"))->GetTransform()->SetWorldPosition(leftElbowPos + glm::vec3(margin * 0.5f, 0, 0)).GetGameObject();
	RigidBody* leftElbowRigidBody = leftElbowGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftElbow"], false,
		glm::inverse(leftElbowRot) * glm::translate(glm::vec3(leftArmToElbow / 2, 0, 0)));

	GameObject* rightArmGO = (new GameObject("rightArm"))->GetTransform()->SetWorldPosition(rightArmPos - glm::vec3(margin * 2.0f, 0, 0)).GetGameObject();
	RigidBody* rightArmRigidBody = rightArmGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightArm"], false,
		glm::translate(glm::vec3(-midSpineToRightArm / 2, 0, 0)));

	glm::mat4x4 rightElbowRot = glm::axisAngleMatrix(glm::vec3(0, 1, 0), glm::radians(90.0f));
	GameObject* rightElbowGO = (new GameObject("rightElbow"))->GetTransform()->SetWorldPosition(rightElbowPos - glm::vec3(margin * 0.5f, 0, 0)).GetGameObject();
	RigidBody* rightElbowRigidBody = rightElbowGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightElbow"], false,
		glm::inverse(rightElbowRot) * glm::translate(glm::vec3(-rightArmToElbow / 2, 0, 0)));

	PhysXJoint* midSpineToLeftArmJoint = upperBodyRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	midSpineToLeftArmJoint->LinkActor(PhysXJoint::Type::D6Joint, upperBodyRigidBody, leftArmRigidBody,
		glm::translate(leftArmGO->GetTransform()->GetWorldPosition() - upperBodyGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxMidSpineToLAJoint = (PxD6Joint*)midSpineToLeftArmJoint->GetPxJoint();
	pxMidSpineToLAJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-180.0f), glm::radians(0.0f), 0.1f));
	pxMidSpineToLAJoint->setSwingLimit(PxJointLimitCone(glm::radians(90.0f), glm::radians(90.0f), 0.1f));
	pxMidSpineToLAJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxMidSpineToLAJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	pxMidSpineToLAJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

	PhysXJoint* leftArmToElbowJoint = leftArmRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	leftArmToElbowJoint->LinkActor(PhysXJoint::Type::D6Joint, leftArmRigidBody, leftElbowRigidBody,
		glm::translate(leftElbowGO->GetTransform()->GetWorldPosition() - leftArmGO->GetTransform()->GetWorldPosition()) * leftElbowRot, glm::mat4x4());
	PxD6Joint* pxLAToElbowJoint = (PxD6Joint*)leftArmToElbowJoint->GetPxJoint();
	pxLAToElbowJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(0.0f), glm::radians(120.0f)));
	pxLAToElbowJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxLAToElbowJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxLAToElbowJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);

	PhysXJoint* midSpineToRightArmJoint = upperBodyRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	midSpineToRightArmJoint->LinkActor(PhysXJoint::Type::D6Joint, upperBodyRigidBody, rightArmRigidBody,
		glm::translate(rightArmGO->GetTransform()->GetWorldPosition() - upperBodyGO->GetTransform()->GetWorldPosition()), glm::mat4x4());
	PxD6Joint* pxMidSpineToRAJoint = (PxD6Joint*)midSpineToRightArmJoint->GetPxJoint();
	pxMidSpineToRAJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-180.0f), glm::radians(0.0f), 0.1f));
	pxMidSpineToRAJoint->setSwingLimit(PxJointLimitCone(glm::radians(90.0f), glm::radians(90.0f), 0.1f));
	pxMidSpineToRAJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxMidSpineToRAJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	pxMidSpineToRAJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

	PhysXJoint* rightArmToElbowJoint = rightArmRigidBody->GetGameObject()->AddComponent<PhysXJoint>();
	rightArmToElbowJoint->LinkActor(PhysXJoint::Type::D6Joint, rightArmRigidBody, rightElbowRigidBody,
		glm::translate(rightElbowGO->GetTransform()->GetWorldPosition() - rightArmGO->GetTransform()->GetWorldPosition()) * rightElbowRot, glm::mat4x4());
	PxD6Joint* pxRAToElbowJoint = (PxD6Joint*)rightArmToElbowJoint->GetPxJoint();
	pxRAToElbowJoint->setTwistLimit(PxJointAngularLimitPair(glm::radians(-120.0f), glm::radians(0.0f)));
	pxRAToElbowJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
	pxRAToElbowJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	pxRAToElbowJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);

	//glm::vec3 scale = GetTransform()->GetLocalScale();
	//GetTransform()->SetLocalScale(glm::vec3(1, 1, 1));

	auto& avatarBoneTrans = avatar->GetBoneTrans();

	//deform = inverse(리지드바디) * 본
	//본 = 리지드바디 * deform * inverse(world??)
	//deform = inverse(리지드바디) * 본
	//본 = 리지드바디 * deform * inverse(world??)
	rootBone.rigidBody = pelvisRigidBody;
	rootBone.deformMat = glm::inverse(pelvisRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix());

	RagdollBone pelvisBone;
	pelvisBone.rigidBody = pelvisRigidBody;
	pelvisBone.deformMat = glm::inverse(pelvisRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.Pelvis->GetWorldMatrix();
	pelvisBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.Pelvis) - avatarBoneTrans.begin();
	ragdollBones.push_back(pelvisBone);

	RagdollBone midSpineBone;
	midSpineBone.rigidBody = upperBodyRigidBody;
	midSpineBone.deformMat = glm::inverse(upperBodyRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.MiddleSpine->GetWorldMatrix();
	midSpineBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.MiddleSpine) - avatarBoneTrans.begin();
	ragdollBones.push_back(midSpineBone);

	RagdollBone leftArmBone;
	leftArmBone.rigidBody = leftArmRigidBody;
	leftArmBone.deformMat = glm::inverse(leftArmRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.LeftArm->GetWorldMatrix();
	leftArmBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.LeftArm) - avatarBoneTrans.begin();
	ragdollBones.push_back(leftArmBone);

	RagdollBone leftElbowBone;
	leftElbowBone.rigidBody = leftElbowRigidBody;
	leftElbowBone.deformMat = glm::inverse(leftElbowRot) * glm::inverse(leftElbowRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.LeftElbow->GetWorldMatrix();
	leftElbowBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.LeftElbow) - avatarBoneTrans.begin();
	ragdollBones.push_back(leftElbowBone);

	RagdollBone rightArmBone;
	rightArmBone.rigidBody = rightArmRigidBody;
	rightArmBone.deformMat = glm::inverse(rightArmRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.RightArm->GetWorldMatrix();
	rightArmBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.RightArm) - avatarBoneTrans.begin();
	ragdollBones.push_back(rightArmBone);

	RagdollBone rightElbowBone;
	rightElbowBone.rigidBody = rightElbowRigidBody;
	rightElbowBone.deformMat = glm::inverse(rightElbowRot) * glm::inverse(rightElbowRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.RightElbow->GetWorldMatrix();
	rightElbowBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.RightElbow) - avatarBoneTrans.begin();
	ragdollBones.push_back(rightElbowBone);

	RagdollBone headBone;
	headBone.rigidBody = headRigidBody;
	headBone.deformMat = glm::inverse(headRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.Head->GetWorldMatrix();
	headBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.Head) - avatarBoneTrans.begin();
	ragdollBones.push_back(headBone);

	RagdollBone leftHeapBone;
	leftHeapBone.rigidBody = leftHipsRigidBody;
	leftHeapBone.deformMat = glm::inverse(leftHipsRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.LeftHips->GetWorldMatrix();
	leftHeapBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.LeftHips) - avatarBoneTrans.begin();
	ragdollBones.push_back(leftHeapBone);

	RagdollBone rightHeapBone;
	rightHeapBone.rigidBody = rightHipsRigidBody;
	rightHeapBone.deformMat = glm::inverse(rightHipsRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.RightHips->GetWorldMatrix();
	rightHeapBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.RightHips) - avatarBoneTrans.begin();
	ragdollBones.push_back(rightHeapBone);

	RagdollBone leftKneeBone;
	leftKneeBone.rigidBody = leftKneeRigidBody;
	leftKneeBone.deformMat = glm::inverse(leftKneeRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.LeftKnee->GetWorldMatrix();
	leftKneeBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.LeftKnee) - avatarBoneTrans.begin();
	ragdollBones.push_back(leftKneeBone);

	RagdollBone rightKneeBone;
	rightKneeBone.rigidBody = rightKneeRigidBody;
	rightKneeBone.deformMat = glm::inverse(rightKneeRigidBody->GetGameObject()->GetTransform()->GetWorldMatrix()) * ragdollInfo.RightKnee->GetWorldMatrix();
	rightKneeBone.boneIndex = std::find(avatarBoneTrans.begin(), avatarBoneTrans.end(), ragdollInfo.RightKnee) - avatarBoneTrans.begin();
	ragdollBones.push_back(rightKneeBone);

	glm::mat4x4 worldMat = GetTransform()->GetWorldMatrix();

	pelvisRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * pelvisRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	upperBodyRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * upperBodyRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	headRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * headRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	leftHipsRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * leftHipsRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	rightHipsRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * rightHipsRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	leftKneeRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * leftKneeRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	rightKneeRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * rightKneeRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	leftArmRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * leftArmRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	leftElbowRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * leftElbowRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	rightArmRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * rightArmRigidBody->GetPxRigidActor()->getGlobalPose()), false);
	rightElbowRigidBody->GetPxRigidActor()->setGlobalPose(PxTransform(GetPxMatrixFromGLMMatrix(worldMat) * rightElbowRigidBody->GetPxRigidActor()->getGlobalPose()), false);

	pelvisRigidBody->Update();
	upperBodyRigidBody->Update();
	headRigidBody->Update();
	leftHipsRigidBody->Update();
	rightHipsRigidBody->Update();
	leftKneeRigidBody->Update();
	rightKneeRigidBody->Update();
	leftArmRigidBody->Update();
	leftElbowRigidBody->Update();
	rightArmRigidBody->Update();
	rightElbowRigidBody->Update();
}

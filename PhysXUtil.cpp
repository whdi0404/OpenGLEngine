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

void PhysXUtil::MakeRagdoll(std::string id, RagdollInfo ragdollInfo)
{
	PxSceneWriteLock lock(*g_PhysXManager->GetScene());

	auto* resources = ResourceManager::GetInstance().GetResource<std::unordered_map<std::string, PxGeometry*>>(id);

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

	float upperBodyToHead = headPos.y -leftArmPos.y;

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

		ResourceManager::GetInstance().AddResource(id, resources);
	}
	//몸통
	GameObject* pelvisGO = (new GameObject("pelvis"))->GetTransform()->SetWorldPosition(ragdollInfo.Pelvis->GetWorldPosition()).GetGameObject();
	RigidBody* pelvisRigidBody = pelvisGO->AddComponent<RigidBody>()->SetGeometry((*resources)["pelvis"], false, glm::translate(glm::vec3(0, (pelvisTop - pelvisBottom) * 0.4f, 0)));

	GameObject* upperBodyGO = (new GameObject("upperBody"))->GetTransform()->SetWorldPosition(ragdollInfo.MiddleSpine->GetWorldPosition()).GetGameObject();
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
	glm::vec3 headWorldPos = ragdollInfo.Head->GetWorldPosition();
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
	GameObject* leftHipsGO = (new GameObject("leftHips"))->GetTransform()->SetWorldPosition(ragdollInfo.LeftHips->GetWorldPosition()).GetGameObject();
	RigidBody* leftHipsRigidBody = leftHipsGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftHips"], false,
		glm::translate(glm::vec3(0, -leftLegToKnee / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* rightHipsGO = (new GameObject("rightHips"))->GetTransform()->SetWorldPosition(ragdollInfo.RightHips->GetWorldPosition()).GetGameObject();
	RigidBody* rightHipsRigidBody = rightHipsGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightHips"], false,
		glm::translate(glm::vec3(0, -rightLegToKnee / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* leftKneeGO = (new GameObject("leftKnee"))->GetTransform()->SetWorldPosition(ragdollInfo.LeftKnee->GetWorldPosition() + glm::vec3(0, -margin * 1.5f * 0.5f, 0)).GetGameObject();
	RigidBody* leftKneeRigidBody = leftKneeGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftKnee"], false,
		glm::translate(glm::vec3(0, -leftKneeToFoot / 2, 0)) *
		glm::axisAngleMatrix(glm::vec3(0, 0, 1), glm::radians(90.0f)));

	GameObject* rightKneeGO = (new GameObject("rightKnee"))->GetTransform()->SetWorldPosition(ragdollInfo.RightKnee->GetWorldPosition() + glm::vec3(0, -margin * 1.5f * 0.5f, 0)).GetGameObject();
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

	GameObject* leftArmGO = (new GameObject("leftArm"))->GetTransform()->SetWorldPosition(ragdollInfo.LeftArm->GetWorldPosition() + glm::vec3(margin * 2.0f, 0, 0)).GetGameObject();
	RigidBody* leftArmRigidBody = leftArmGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftArm"], false,
		glm::translate(glm::vec3(midSpineToLeftArm / 2, 0, 0)));

	glm::mat4x4 leftElbowRot = glm::axisAngleMatrix(glm::vec3(0, 1, 0), glm::radians(90.0f));
	GameObject* leftElbowGO = (new GameObject("leftElbow"))->GetTransform()->SetWorldPosition(ragdollInfo.LeftElbow->GetWorldPosition() + glm::vec3(margin * 0.5f, 0, 0)).GetGameObject();
	RigidBody* leftElbowRigidBody = leftElbowGO->AddComponent<RigidBody>()->SetGeometry((*resources)["leftElbow"], false,
		glm::inverse(leftElbowRot) * glm::translate(glm::vec3(leftArmToElbow / 2, 0, 0)));

	GameObject* rightArmGO = (new GameObject("rightArm"))->GetTransform()->SetWorldPosition(ragdollInfo.RightArm->GetWorldPosition() - glm::vec3(margin * 2.0f, 0, 0)).GetGameObject();
	RigidBody* rightArmRigidBody = rightArmGO->AddComponent<RigidBody>()->SetGeometry((*resources)["rightArm"], false,
		glm::translate(glm::vec3(-midSpineToRightArm / 2, 0, 0)));

	glm::mat4x4 rightElbowRot = glm::axisAngleMatrix(glm::vec3(0, 1, 0), glm::radians(90.0f));
	GameObject* rightElbowGO = (new GameObject("rightElbow"))->GetTransform()->SetWorldPosition(ragdollInfo.RightElbow->GetWorldPosition() - glm::vec3(margin * 0.5f, 0, 0)).GetGameObject();
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
}

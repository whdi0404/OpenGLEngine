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
	if (resources == nullptr)
	{
		resources = new std::unordered_map<std::string, PxGeometry*>();

		{
			glm::vec3 localPos = ragdollInfo.Pelvis->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(2.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("pelvis", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.LeftHips->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("leftHips", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.RightHips->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("rightHips", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.LeftKnee->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("leftKnee", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.RightKnee->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("rightKnee", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.LeftFoot->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("leftFoot", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.RightFoot->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("rightFoot", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.LeftArm->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("leftArm", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.RightArm->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("rightArm", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.LeftElbow->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("leftElbow", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.RightElbow->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("rightElbow", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.MiddleSpine->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("middleSpine", geom));
		}
		{
			glm::vec3 localPos = ragdollInfo.Head->GetLocalPosition();
			PxCapsuleGeometry* geom = new PxCapsuleGeometry(1.0f, glm::length(localPos) / 2);

			resources->insert(std::make_pair("head", geom));
		}

		ResourceManager::GetInstance().AddResource(id, resources);
	}
	RigidBody* pelvisRigidBody = (new GameObject("pelvis"))->GetTransform()->SetParent(ragdollInfo.Pelvis, false).GetGameObject()->
		AddComponent<RigidBody>()->SetGeometry((*resources)["pelvis"], false);

	RigidBody* leftHipsRigidBody = (new GameObject("leftHips"))->GetTransform()->SetParent(ragdollInfo.LeftHips, false).GetGameObject()->
		AddComponent<RigidBody>()->SetGeometry((*resources)["leftHips"], false);

	//RigidBody* leftKneeRigidBody = (new GameObject("leftKnee"))->GetTransform()->SetParent(ragdollInfo.LeftKnee, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["leftKnee"], true);

	//RigidBody* leftFootRigidBody = (new GameObject("leftFoot"))->GetTransform()->SetParent(ragdollInfo.LeftFoot, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["leftFoot"], true);

	//RigidBody* leftArmRigidBody = (new GameObject("leftArm"))->GetTransform()->SetParent(ragdollInfo.LeftArm, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["leftArm"], true);

	//RigidBody* leftElbowRigidBody = (new GameObject("leftElbow"))->GetTransform()->SetParent(ragdollInfo.LeftElbow, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["leftElbow"], true);

	//RigidBody* rightHipsRigidBody = (new GameObject("rightHips"))->GetTransform()->SetParent(ragdollInfo.RightHips, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["rightHips"], true);

	//RigidBody* rightKneeRigidBody = (new GameObject("rightKnee"))->GetTransform()->SetParent(ragdollInfo.RightKnee, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["rightKnee"], true);

	//RigidBody* rightFootRigidBody = (new GameObject("rightFoot"))->GetTransform()->SetParent(ragdollInfo.RightFoot, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["rightFoot"], true);

	//RigidBody* rightArmRigidBody = (new GameObject("rightArm"))->GetTransform()->SetParent(ragdollInfo.RightArm, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["rightArm"], true);

	//RigidBody* rightElbowRigidBody = (new GameObject("rightElbow"))->GetTransform()->SetParent(ragdollInfo.RightElbow, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["rightElbow"], true);

	//RigidBody* middleSpineRigidBody = (new GameObject("middleSpine"))->GetTransform()->SetParent(ragdollInfo.MiddleSpine, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["middleSpine"], true);

	//RigidBody* headRigidBody = (new GameObject("Head"))->GetTransform()->SetParent(ragdollInfo.Head, false).GetGameObject()->
	//	AddComponent<RigidBody>()->SetGeometry((*resources)["head"], true);

	{
		glm::mat4x4 mat = ragdollInfo.LeftHips->GetWorldMatrix();
		mat = glm::transpose(mat);
		
		PxMat44 pxMat1;
		pxMat1[0] = GetPxVec4FromGLMVec4(mat[0]);
		pxMat1[1] = GetPxVec4FromGLMVec4(mat[1]);
		pxMat1[2] = GetPxVec4FromGLMVec4(mat[2]);
		pxMat1[3] = GetPxVec4FromGLMVec4(mat[3]);

		
		//mat2 = glm::transpose(mat2);

		PxMat44 pxMat2 = PxMat44(PxVec4(5, 0, 0,1));
		//어케 해줘야대...
		
		PxTransform trans1 = PxTransform(PxMat44(PxIdentity));
		PxTransform trans2 = PxTransform(pxMat2);
		leftHipsRigidBody->GetGameObject()->AddComponent<PhysXJoint>()->LinkActor(PhysXJoint::Type::FixedJoint, leftHipsRigidBody, trans1, pelvisRigidBody, trans2);
	}
}

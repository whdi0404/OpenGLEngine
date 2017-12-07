#include "stdafx.h"
#include "PhysXManager.h"
#include "Time.h"
#include "Gizmo.h"

PhysXManager::PhysXManager() : pvd(nullptr), mStepSize(0.016666660f), fixedStepper(0.016666660f, 8), waitForResults(false), gpuMaxNumPartitions(32)
{
	initSampleAllocator();

	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, GetDefaultAllocator(), GetDefaultErrorCallback());
	if (foundation == nullptr)
		std::cout << "PxCreateFoundation failed!" << std::endl;

	PxCudaContextManagerDesc cudaContextManagerDesc = {};
	
	cudaContextManager = PxCreateCudaContextManager(*foundation, cudaContextManagerDesc);
	if (cudaContextManager != nullptr)
	{
		if (!cudaContextManager->contextIsValid())
		{
			cudaContextManager->release();
			cudaContextManager = nullptr;
		}
	}
	PxTolerancesScale scale;

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale);

	if (physics == nullptr)
		std::cout << "PxCreatePhysics failed!" << std::endl;

	if (!PxInitExtensions(*physics, pvd))
		std::cout << "PxInitExtensions failed!" << std::endl;

	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.
	
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params);
	if (!mCooking)
		std::cout << "PxCreateCooking failed!" << std::endl;

	physics->registerDeletionListener(*this, PxDeletionEventFlag::eUSER_RELEASE);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	GetDefaultSceneDesc(sceneDesc);

	if (!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
			std::cout << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

#if PX_SUPPORT_GPU_PHYSX
	if (!sceneDesc.gpuDispatcher && cudaContextManager)
	{
		auto gpuCudaDispatcher = cudaContextManager->getGpuDispatcher();
		sceneDesc.gpuDispatcher = gpuCudaDispatcher;
	}
#endif

	//sceneDesc.frictionType = PxFrictionType::eTWO_DIRECTIONAL;
	//sceneDesc.frictionType = PxFrictionType::eONE_DIRECTIONAL;
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_AVERAGE_POINT;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	//sceneDesc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	sceneDesc.flags |= PxSceneFlag::eSUPPRESS_EAGER_SCENE_QUERY_REFIT;
	sceneDesc.flags |= PxSceneFlag::eDISABLE_CONTACT_CACHE;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = gpuMaxNumPartitions;


	//customizeSceneDesc(sceneDesc);

	mScene = physics->createScene(sceneDesc);
	if (!mScene)
		std::cout << "createScene failed!" << std::endl;

	PxSceneWriteLock scopedLock(*mScene);

	PxSceneFlags flag = mScene->getFlags();

	PX_UNUSED(flag);
	mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f/*0.0f*/);
	mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

#ifdef USE_MBP
	setupMBP(*mScene);
#endif
}


PhysXManager::~PhysXManager()
{
	releaseSampleAllocator();
}

void PhysXManager::PreRender()
{
	Stepper* stepper = &fixedStepper;

	waitForResults = false;

	float dtime = Time::GetInstance().GetDeltaTime();
	if (mScene)
	{
#if !PX_PROFILE
		waitForResults = stepper->advance(mScene, dtime, nullptr, 0);

		// tells the stepper shape data is not going to be accessed until next frame 
		// (frame ends with stepper->wait(mScene))
		stepper->renderDone();

#else
		// in profile builds we run the whole frame sequentially
		// simulate, wait, update render objects, render
		{
			waitForResults = stepper->advance(mScene, dtime, mScratchBlock, SCRATCH_BLOCK_SIZE);
			stepper->renderDone();
			if (waitForResults)
			{
				stepper->wait(mScene);
				mSimulationTime = stepper->getSimulationTime();
			}
		}

		// update render objects immediately
		if (waitForResults)
		{
			bufferActiveTransforms();
			updateRenderObjectsSync(dtime);
			if (mOneFrameUpdate)
				updateRenderObjectsAsync(dtime);
		}
#endif
	}
}

void PhysXManager::DrawGizmos(Camera* camera)
{
	RenderData(camera, mScene->getRenderBuffer());
}

struct convert_vec3
{
	template<typename T>
	glm::vec3&& operator() (T&& val)
	{
		return glm::vec3(val.x, val.y, val.z);
	}
};
void PhysXManager::RenderData(Camera* camera, const PxRenderBuffer & data)
{
	glLineWidth(1.0f);
	glDisable(GL_LIGHTING);

	//----------Render Points------------------
	unsigned int NbPoints = data.getNbPoints();
	if (NbPoints)
	{
		float* pVertList = new float[NbPoints * 3];
		float* pColorList = new float[NbPoints * 4];
		int vertIndex = 0;
		int colorIndex = 0;
		const PxDebugPoint* Points = data.getPoints();
		while (NbPoints--)
		{
			pVertList[vertIndex++] = Points->pos.x;
			pVertList[vertIndex++] = Points->pos.y;
			pVertList[vertIndex++] = Points->pos.z;
			pColorList[colorIndex++] = (float)((Points->color >> 16) & 0xff) / 255.0f;
			pColorList[colorIndex++] = (float)((Points->color >> 8) & 0xff) / 255.0f;
			pColorList[colorIndex++] = (float)(Points->color & 0xff) / 255.0f;
			pColorList[colorIndex++] = 1.0f;
			Points++;
		}

		RenderBuffer(pVertList, pColorList, GL_POINTS, data.getNbPoints());

		delete[] pVertList;
		delete[] pColorList;
	}


	//----------Render Lines------------------
	unsigned int NbLines = data.getNbLines();
	if (NbLines)
	{
		//float* pVertList = new float[NbLines * 3 * 2];
		//float* pColorList = new float[NbLines * 4 * 2];
		int vertIndex = 0;
		int colorIndex = 0;
		const PxDebugLine* Lines = data.getLines();

		
		convert_vec3 converter;
		static std::vector<glm::vec3> lines;
		//startTime = timeInstance.GetNowTimeSinceStart();
		while (NbLines--)
		{
			lines.emplace_back(std::move(converter(Lines->pos0)));
			lines.emplace_back(std::move(converter(Lines->pos1)));
			//pVertList[vertIndex++] = Lines->pos0.x;
			//pVertList[vertIndex++] = Lines->pos0.y;
			//pVertList[vertIndex++] = Lines->pos0.z;
			//pColorList[colorIndex++] = (float)((Lines->color0 >> 16) & 0xff) / 255.0f;
			//pColorList[colorIndex++] = (float)((Lines->color0 >> 8) & 0xff) / 255.0f;
			//pColorList[colorIndex++] = (float)(Lines->color0 & 0xff) / 255.0f;
			//pColorList[colorIndex++] = 1.0f;
			//
			//pVertList[vertIndex++] = Lines->pos1.x;
			//pVertList[vertIndex++] = Lines->pos1.y;
			//pVertList[vertIndex++] = Lines->pos1.z;
			//pColorList[colorIndex++] = (float)((Lines->color0 >> 16) & 0xff) / 255.0f;
			//pColorList[colorIndex++] = (float)((Lines->color0 >> 8) & 0xff) / 255.0f;
			//pColorList[colorIndex++] = (float)(Lines->color0 & 0xff) / 255.0f;
			//pColorList[colorIndex++] = 1.0f;

			Lines++;
		}
		Gizmo::DrawLines(camera, lines);
		lines.clear();

		//RenderBuffer(pVertList, pColorList, GL_LINES, data.getNbLines() * 2);

		//delete[] pVertList;
		//delete[] pColorList;
	}


	//----------Render Triangles------------------
	unsigned int NbTris = data.getNbTriangles();
	if (NbTris)
	{
		float* pVertList = new float[NbTris * 3 * 3];
		float* pColorList = new float[NbTris * 4 * 3];
		int vertIndex = 0;
		int colorIndex = 0;
		const PxDebugTriangle* Triangles = data.getTriangles();
		while (NbTris--)
		{
			pVertList[vertIndex++] = Triangles->pos0.x;
			pVertList[vertIndex++] = Triangles->pos0.y;
			pVertList[vertIndex++] = Triangles->pos0.z;

			pVertList[vertIndex++] = Triangles->pos1.x;
			pVertList[vertIndex++] = Triangles->pos1.y;
			pVertList[vertIndex++] = Triangles->pos1.z;

			pVertList[vertIndex++] = Triangles->pos2.x;
			pVertList[vertIndex++] = Triangles->pos2.y;
			pVertList[vertIndex++] = Triangles->pos2.z;

			for (int i = 0; i<3; i++)
			{
				pColorList[colorIndex++] = (float)((Triangles->color0 >> 16) & 0xff) / 255.0f;
				pColorList[colorIndex++] = (float)((Triangles->color0 >> 8) & 0xff) / 255.0f;
				pColorList[colorIndex++] = (float)(Triangles->color0 & 0xff) / 255.0f;
				pColorList[colorIndex++] = 1.0f;
			}

			Triangles++;
		}

		RenderBuffer(pVertList, pColorList, GL_TRIANGLES, data.getNbTriangles() * 3);

		delete[] pVertList;
		delete[] pColorList;
	}
	glEnable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void PhysXManager::RenderBuffer(float* pVertList, float* pColorList, int type, int num)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pVertList);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, pColorList);

	glDrawArrays(type, 0, num);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysXManager::PostRender()
{
	if (mScene && waitForResults)
	{
		Stepper* stepper = &fixedStepper;
		stepper->wait(mScene);
		//mSimulationTime = stepper->getSimulationTime();

		//bufferActiveTransforms();

		//// make sure that in single step mode, the render objects get updated immediately
		//if (mOneFrameUpdate)
		//{
		//	updateRenderObjectsSync(dtime);
		//	updateRenderObjectsAsync(dtime);
		//}
		//else
		//	updateRenderObjectsSync(dtime);

	}
}

void PhysXManager::onRelease(const PxBase * observed, void * userData, PxDeletionEventFlag::Enum deletionEvent)
{
	PX_UNUSED(userData);
	PX_UNUSED(deletionEvent);
}

PX_FORCE_INLINE void SetupDefaultRigidDynamic(PxRigidDynamic& body, bool kinematic = false)
{
	body.setActorFlag(PxActorFlag::eVISUALIZATION, true);
	body.setAngularDamping(0.5f);
	body.setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
}

PhysXActor::PhysXActor() : mMaterial(nullptr), actor(nullptr), rigidDynamic(nullptr)
{
}

PhysXActor::~PhysXActor()
{
}

void PhysXActor::Update()
{
	PxScene* pxScene = g_PhysXManager->GetScene();
	PxSceneWriteLock scopedLock(*pxScene);
	if (rigidDynamic == nullptr)
		return;
	PxTransform globalPose = rigidDynamic->getGlobalPose();
	this->GetTransform()->SetRotateLocal(glm::quat(globalPose.q.w, globalPose.q.x, globalPose.q.y, globalPose.q.z));
	this->GetTransform()->SetLocalPosition(globalPose.p.x, globalPose.p.y, globalPose.p.z);
}

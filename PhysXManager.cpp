#include "stdafx.h"
#include "PhysXManager.h"
#include "Time.h"

PhysXManager::PhysXManager() : pvd(nullptr), mStepSize(0.016666660f), fixedStepper(0.016666660f, 8), waitForResults(false), gpuMaxNumPartitions(512)
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
	//sceneDesc.flags |= PxSceneFlag::eDISABLE_CONTACT_CACHE;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = gpuMaxNumPartitions;


#ifdef USE_MBP
	sceneDesc.broadPhaseType = PxBroadPhaseType::eMBP;
#endif

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

void PhysXActor::CreateToBox()
{
	PxPhysics* physics = g_PhysXManager->GetPhysics();
	PxScene* pxScene = g_PhysXManager->GetScene();
	
	mMaterial = physics->createMaterial(0.5f, 0.5f, 0.1f);
	
	PxSceneWriteLock scopedLock(*pxScene);
	
	PxVec3 worldPos = GetPxVec3FromGLMVec3(GetTransform()->GetWorldPosition());
	rigidDynamic = PxCreateDynamic(*physics, PxTransform(worldPos), PxBoxGeometry(PxVec3(0, 1, 0)), *mMaterial, 20.0f);
	PX_ASSERT(rigidDynamic);
	
	SetupDefaultRigidDynamic(*rigidDynamic);
	pxScene->addActor(*rigidDynamic);
	
	rigidDynamic->setLinearVelocity(PxVec3(1, 1, 1));
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

#pragma once
#include "SampleStepper.h"

using namespace physx;
class PhysXManager : public PxDeletionListener
{
private:
	PxFoundation* foundation;
	PxCudaContextManager* cudaContextManager;
	PxPvdTransport*                  transport;
	PxPvdInstrumentationFlags        pvdFlags;
	PxPvd* pvd;
	PxDefaultCpuDispatcher* mCpuDispatcher;
	GetMacro(PxCooking*, Cookie, mCooking);
	FixedStepper fixedStepper;

	GetMacro(PxPhysics*, Physics, physics);
	PxMaterial* mMaterial;
	GetMacro(PxScene*, Scene, mScene);
	PxReal mStepSize;


	int gpuMaxNumPartitions;
	bool waitForResults;

public:
	PhysXManager();
	~PhysXManager();

	static PxErrorCallback& GetDefaultErrorCallback()
	{
		static PxDefaultErrorCallback gDefaultErrorCallback;
		return gDefaultErrorCallback;
	}

	static PxAllocatorCallback& GetDefaultAllocator()
	{
		static PxDefaultAllocator gDefaultAllocator;
		return gDefaultAllocator;
	}

	static void GetDefaultSceneDesc(PxSceneDesc&) {}

	void PreRender();

	void DrawGizmos(Camera* camera);

	void RenderData(Camera* camera, const PxRenderBuffer & data);

	void RenderBuffer(float* pVertList, float* pColorList, int type, int num);

	void PostRender();

	virtual void onRelease(const PxBase* observed, void* userData, PxDeletionEventFlag::Enum deletionEvent);
};

class PhysXActor : public Component
{
private:
	PxMaterial* mMaterial;
	PxRigidActor* actor;
	PxRigidDynamic* rigidDynamic;

public:
	PhysXActor();
	~PhysXActor();

public:
	void CreateToBox();
	virtual void Update();
};
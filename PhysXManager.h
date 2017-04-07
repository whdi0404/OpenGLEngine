#pragma once
#include "SingletonBase.h"
class PhysXManager :
	public Singleton<PhysXManager>
{
private:
	PxFoundation* foundation;
	PxCudaContextManager* cudaContextManager;
	PxPvdTransport*                  transport;
	PxPvdInstrumentationFlags        pvdFlags;
	PxPvd* pvd;
	CUcontext* cuContext;

	PxPhysics*								physics;

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
};
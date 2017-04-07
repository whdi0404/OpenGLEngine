#include "stdafx.h"
#include "PhysXManager.h"
#include <cudamanager\PxCudaContextManager.h>

PhysXManager::PhysXManager() : pvd(nullptr)
{	
	std::cout << "??" << std::endl;
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, GetDefaultAllocator(), GetDefaultErrorCallback());
	if (foundation != nullptr)
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

	if (!physics)
		std::cout << "PxCreatePhysics failed!" << std::endl;

	if (!PxInitExtensions(*physics, pvd))
		std::cout << "PxInitExtensions failed!" << std::endl;
}


PhysXManager::~PhysXManager()
{
}
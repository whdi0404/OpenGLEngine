#pragma once
#include "Component.h"
class RigidBody :
	public Component
{
public:
	RigidBody();
	~RigidBody();

	virtual void Initialize() override;
	virtual void Update()  override;
	virtual void OnDrawGizmo() override;
	virtual void Release()  override;
};
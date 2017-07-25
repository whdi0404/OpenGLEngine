#pragma once
#include "Component.h"
#include "Time.h"
#include "Gizmo.h"

class TestMoving :
	public Component
{
private:

public:
	TestMoving();
	~TestMoving();


	virtual void Update();
	
	virtual void OnDrawGizmo();

	SetMacro(Camera*, Camera, camera);
};


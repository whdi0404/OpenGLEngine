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


	virtual void Update()
	{
		float dt = Time::GetInstance().GetDeltaTime();
		GetTransform()->AddWorldPosition(glm::vec3(0, 0, -5 * dt));
	}
	
	virtual void OnDrawGizmo();

	SetMacro(Camera*, Camera, camera);
};


#include "stdafx.h"
#include "TestMoving.h"
#include "GameObject.h"
#include "RenderObject.h"

TestMoving::TestMoving() : camera(nullptr)
{
}


TestMoving::~TestMoving()
{
}

void TestMoving::Update()
{
	float dt = Time::GetInstance().GetDeltaTime();
	float speed = 5 * dt;
	GetTransform()->AddLocalPosition(speed, speed, speed);
}

void TestMoving::OnDrawGizmo()
{
	//RenderObject* renderObject = GetGameObject()->GetComponent<RenderObject>();
	//if (camera == nullptr || renderObject == nullptr)
	//	return;
	//
	//Gizmo::DrawSphere(camera, GetGameObject()->GetCullSphere().center, GetGameObject()->GetCullSphere().radius);
}

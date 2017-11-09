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
	this->GetTransform()->AddWorldPosition(glm::vec3(10.0f * Time::GetInstance().GetDeltaTime(),0,0));
}

void TestMoving::OnDrawGizmo()
{
	auto& cullSphere = GetGameObject()->GetCullSphere();
	Gizmo::DrawSphere(camera, cullSphere.center, cullSphere.radius,4);
}

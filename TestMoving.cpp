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

void TestMoving::OnDrawGizmo()
{
	//RenderObject* renderObject = GetGameObject()->GetComponent<RenderObject>();
	//if (camera == nullptr || renderObject == nullptr)
	//	return;
	//
	//Gizmo::DrawSphere(camera, GetGameObject()->GetCullSphere().center, GetGameObject()->GetCullSphere().radius);
}

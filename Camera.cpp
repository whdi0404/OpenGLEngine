#include "stdafx.h"
#include "Camera.h"

using namespace Math;

Camera* Camera::mainCamera = nullptr;

Camera::Camera() : FOVDeg(45.0f), projectionType(ProjectionType::Perspective),
fNear(0.1f), fFar(20000.0f)
{
}

Camera::~Camera()
{
}

void Camera::Initialize()
{
	if (mainCamera == nullptr)
		mainCamera = this;

	RenderObject::Initialize();
	RefreshProjection();
	RefreshViewMatrix();
}

void Camera::Update()
{
	RefreshViewMatrix();
}

void Camera::RefreshProjection()
{
	if (projectionType == ProjectionType::Perspective)
	{
		int width, height;
		glfwGetWindowSize(g_Window, &width, &height);

		float fovRad = glm::radians(FOVDeg);
		float aspect = (float)width / (float)height;
		matProj = glm::perspective(fovRad, aspect, fNear, fFar);
		frustum.setCamInternals(FOVDeg, aspect, fNear, fFar);
		RefreshFrustumPlanes();
	}
}

void Camera::RefreshViewMatrix()
{
	matView = glm::inverse(GetTransform()->GetWorldMatrix());
	RefreshFrustumPlanes();
}

bool Camera::IsIn(RenderObject * renderObject)
{
	return (frustum.pointInFrustum(renderObject->GetTransform()->GetWorldPosition()));
}

void Camera::RefreshFrustumPlanes()
{
	Transform* transform = GetTransform();
	frustum.setCamDef(*transform);
}

Camera * Camera::GetMainCamera()
{
	return mainCamera;
}

void Camera::Render()
{
	
}
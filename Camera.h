#pragma once
#include "RenderObject.h"

class Camera :
	public RenderObject
{
public:
	enum ProjectionType {Perspective = 0, Orthographic};

private:
	float FOVDeg;
	float fNear, fFar;
	ProjectionType projectionType;
	Get(mat4x4, MatProj, matProj);
	Get(mat4x4, MatView, matView);

	FrustumG frustum;
public:
	Camera();
	~Camera();

	void SetFOV(float degree)
	{
		FOVDeg = degree;
		RefreshProjection();
	}
	void SetProjectionType(ProjectionType type)
	{
		projectionType = type;
		RefreshProjection();
	}
	void SetNear(float value)
	{
		this->fNear = value;
		RefreshProjection();
	}
	void SetFar(float value)
	{
		this->fNear = value;
		RefreshProjection();
	}

	void RefreshProjection();
	void RefreshViewMatrix();

	bool IsIn(RenderObject* renderObject);
	void Render();

private:
	void RefreshFrustumPlanes();
};
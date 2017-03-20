#pragma once
#include "Camera.h"
static class Gizmo
{
private:
	static glm::vec3 color;
	static int lineThick;
	static glm::vec3 oldPointPos;

private:
	Gizmo() {};
	~Gizmo() {};
public:
	static void Initialize()
	{
		SetColor(color);
		SetThick(lineThick);
	}

	static glm::vec3 GetColor() { return color; }
	static void SetColor(float r, float g, float b, float a = 1)
	{
		color.r = r;
		color.g = g;
		color.b = b;
		glColor3f(Gizmo::color.r, Gizmo::color.g, Gizmo::color.b);
	}
	static void SetColor(glm::vec3 color)
	{
		SetColor(color.r, color.g, color.b);
	}

	static int GetThick() { return lineThick; }
	static void SetThick(int thick)
	{
		lineThick = thick;
		glLineWidth(lineThick);
	}

	static void DrawLine(Camera* cam, glm::vec3 from, glm::vec3 to)
	{
		glBegin(GL_LINES);
		glm::vec4 camPosFrom = GetPointFromCamera(cam, from);
		glm::vec4 camPosTo = GetPointFromCamera(cam, to);

		glVertex4f(camPosFrom.x, camPosFrom.y, camPosFrom.z, camPosFrom.w);
		glVertex4f(camPosTo.x, camPosTo.y, camPosTo.z, camPosTo.w);
		glEnd();

		oldPointPos = to;
	}

	static void DrawLineFromPrevDot(Camera* cam, glm::vec3 to)
	{
		glBegin(GL_LINES);
		glm::vec4 camPosFrom = GetPointFromCamera(cam, oldPointPos);
		glm::vec4 camPosTo = GetPointFromCamera(cam, to);

		glVertex4f(camPosFrom.x, camPosFrom.y, camPosFrom.z, camPosFrom.w);
		glVertex4f(camPosTo.x, camPosTo.y, camPosTo.z, camPosTo.w);
		glEnd();

		oldPointPos = to;
	}

	static glm::vec4 GetPointFromCamera(Camera* cam, glm::vec3 point)
	{
		return cam->GetMatProj() * cam->GetMatView() * glm::vec4(point.x, point.y, point.z, 1);
	}

};
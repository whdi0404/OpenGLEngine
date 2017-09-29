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

	static void DrawSphere(Camera* cam, glm::vec3 center, float radius, int gradation = 5)
	{
		const float PI = 3.141592f;
		glm::vec3 p;
		glm::vec4 r;
		float alpha, beta;
		float pi = glm::pi<float>();


		bool isCulling = glIsEnabled(GL_CULL_FACE);
		if (isCulling == true)
			glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		for (alpha = 0.0f; alpha < pi; alpha += PI / gradation)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for (beta = 0.0f; beta < 2.01f * pi; beta += PI / gradation)
			{
				p.x = radius*cos(beta)*sin(alpha);
				p.y = radius*sin(beta)*sin(alpha);
				p.z = radius*cos(alpha);
				r = GetPointFromCamera(cam, p + center);
				glVertex4f(r.x, r.y, r.z, r.w);

				p.x = radius*cos(beta)*sin(alpha + PI / gradation);
				p.y = radius*sin(beta)*sin(alpha + PI / gradation);
				p.z = radius*cos(alpha + PI / gradation);
				r = GetPointFromCamera(cam, p + center);
				glVertex4f(r.x, r.y, r.z, r.w);
			}
			glEnd();
		}

		if (isCulling == true)
			glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
};
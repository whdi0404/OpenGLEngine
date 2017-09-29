#include "stdafx.h"
#include "math.h"
#include "VertexBuffer.h"

using namespace Math;

void Math::Plane::Normalize()
{
	float Distance = sqrtf(a * a + b * b + c * c);
	a = a / Distance;
	b = b / Distance;
	c = c / Distance;
	d = d / Distance;
}

float Math::Plane::PlaneDot(glm::vec3 point)
{
	return (a * point.x + b * point.y + c * point.z + d);
}

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD)
{
	// store the information
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	// compute width and height of the near and far plane sections
	tang = (float)tan(ANG2RAD * angle * 0.5);
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD  * tang;
	fw = fh * ratio;
}

void FrustumG::setCamDef(Transform& trans)
{
	glm::vec3 dir, nc, fc, X, Y, Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction
	Z = trans.GetBack();

	// X axis of camera with given "up" vector and Z axis
	X = trans.GetRight();

	// the real "up" vector is the cross product of Z and X
	Y = trans.GetUp();

	// compute the centers of the near and far planes
	nc = trans.GetWorldPosition() - Z * nearD;
	fc = trans.GetWorldPosition() - Z * farD;

	// compute the 4 corners of the frustum on the near plane
	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	// compute the 4 corners of the frustum on the far plane
	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	// compute the six planes
	// the function set3Points assumes that the points
	// are given in counter clockwise order
	pl[TOP] = Plane::PlaneFromPoints(ntr, ntl, ftl);
	pl[BOTTOM] = Plane::PlaneFromPoints(nbl, nbr, fbr);
	pl[LEFT] = Plane::PlaneFromPoints(ntl, nbl, fbl);
	pl[RIGHT] = Plane::PlaneFromPoints(nbr, ntr, fbr);
	pl[NEARP] = Plane::PlaneFromPoints(ntl, ntr, nbr);
	pl[FARP] = Plane::PlaneFromPoints(ftr, ftl, fbl);
}

int FrustumG::pointInFrustum(glm::vec3 & p)
{
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		float dist = pl[i].PlaneDot(p);
		if (dist < 0)
			return OUTSIDE;
	}
	return(result);
}

int FrustumG::sphereInFrustum(Math::Sphere& sphere)
{
	float distance;
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		distance = pl[i].PlaneDot(sphere.center);
		if (distance < -sphere.radius)
			return OUTSIDE;
		else if (distance < sphere.radius)
			result = INTERSECT;
	}
	return(result);
}

int FrustumG::boxInFrustum(Math::Box& b)
{
	int result = INSIDE, out, in;

	// for each plane do ...
	for (int i = 0; i < 6; i++) {
		// reset counters for corners in and out
		out = 0; in = 0;
		// for each corner of the box do ...
		// get out of the cycle as soon as a box as corners
		// both inside and out of the frustum
		for (int k = 0; k < 8 && (in == 0 || out == 0); k++) {
			// is the corner outside or inside
			if (pl[i].PlaneDot(b.getVertex(k)) < 0)
				out++;
			else
				in++;
		}
		//if all corners are out
		if (!in)
			return (OUTSIDE);
		// if some corners are out and others are in
		else if (out)
			result = INTERSECT;
	}
	return(result);
}

bool Math::Box::Contain(Sphere & sphere)
{
	if (halfSize.x < sphere.radius ||
		halfSize.y < sphere.radius ||
		halfSize.z < sphere.radius)
		return false;

	glm::vec3 boxMin = center - halfSize;
	glm::vec3 boxMax = center + halfSize;

	glm::vec3 sphereMin = sphere.center - sphere.radius;
	glm::vec3 sphereMax = sphere.center + sphere.radius;

	return sphereMin.x >= boxMin.x &&
		sphereMax.x <= boxMax.x &&

		sphereMin.y >= boxMin.y &&
		sphereMax.y <= boxMax.y &&

		sphereMin.z >= boxMin.z &&
		sphereMax.z <= boxMax.z;
}

bool Math::Box::Collision(Box & boundingBox)
{
	return false;
}

glm::vec3 Math::Box::getVertex(int corner)
{
	switch (corner)
	{
	case 0:
		return center + glm::vec3(halfSize.x, halfSize.y, halfSize.z);
	case 1:
		return center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z);
	case 2:
		return center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z);
	case 3:
		return center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z);
	case 4:
		return center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z);
	case 5:
		return center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z);
	case 6:
		return center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z);
	case 7:
		return center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z);
	}
}

bool Math::Sphere::Collision(Sphere & sphere)
{
	return glm::length(sphere.center - center) <= sphere.radius + radius;
}

Sphere Math::Sphere::CreateBoundingSphere(VertexBuffer * vertexBuffer)
{
	if (vertexBuffer->GetAttribute()->HasElement(Element::Position) == false)
		return Sphere();

	glm::vec3 xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = ymin = zmin = glm::vec3(0, 0, 0) * FBXSDK_FLOAT_MAX;
	xmax = ymax = zmax = glm::vec3(0, 0, 0) * FBXSDK_FLOAT_MIN;

	int vertexCount = vertexBuffer->GetVertexCount();
	for (int i = 0; i < vertexCount; ++i)
	{
		glm::vec4 p = vertexBuffer->GetVector(Element::Position, i);
		if (p.x < xmin.x) xmin = glm::vec3(p.x, p.y, p.z);
		if (p.x > xmax.x) xmax = glm::vec3(p.x, p.y, p.z);
		if (p.y < ymin.y) ymin = glm::vec3(p.x, p.y, p.z);
		if (p.y > ymax.y) ymax = glm::vec3(p.x, p.y, p.z);
		if (p.z < zmin.z) zmin = glm::vec3(p.x, p.y, p.z);
		if (p.z > zmax.z) zmax = glm::vec3(p.x, p.y, p.z);
	}
	auto xSpan = glm::length((xmax - xmin));
	auto ySpan = glm::length((ymax - ymin));
	auto zSpan = glm::length((zmax - zmin));
	auto dia1 = xmin;
	auto dia2 = xmax;
	auto maxSpan = xSpan;

	if (ySpan > maxSpan)
	{
		maxSpan = ySpan;
		dia1 = ymin; dia2 = ymax;
	}
	if (zSpan > maxSpan)
	{
		dia1 = zmin; dia2 = zmax;
	}

	auto center = (dia1 + dia2) * 0.5f;
	float radius = glm::length(dia2 - center);
	for (int i = 0; i < vertexCount; ++i)
	{
		glm::vec4 p4 = vertexBuffer->GetVector(Element::Position, i);
		glm::vec3 p = glm::vec3(p4.x, p4.y, p4.z);
		float d = glm::length(p - center);
		if (d > radius)
		{
			auto r = d;
			radius = (radius + r) * 0.5f;
			auto offset = r - radius;
			center = (radius * center + offset * p) / r;
		}
	}

	return Sphere{ center, radius };
}
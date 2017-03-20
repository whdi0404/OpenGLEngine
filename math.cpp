#include "stdafx.h"
#include "math.h"

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

int FrustumG::sphereInFrustum(glm::vec3 & p, float radius)
{
	float distance;
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		distance = pl[i].PlaneDot(p);
		if (distance < -radius)
			return OUTSIDE;
		else if (distance < radius)
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

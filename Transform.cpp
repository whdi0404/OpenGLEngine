#include "stdafx.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "Scene.h"
#include "Octree.h"
#include "GameObject.h"

Transform::Transform() : localPosition(0, 0, 0), localScale(1, 1, 1), worldMatrix(1.0f),
parent(nullptr), gameObject(nullptr)
{
	this->forward = vec3(0, 0, 1);
	this->up = vec3(0, 1, 0);
	this->right = vec3(1, 0, 0);

	localMatrix = worldMatrix = mat4x4();
}


Transform::~Transform()
{
}

Transform & Transform::SetPosition(float x, float y, float z)
{
	mat4x4 newWorldmat = worldMatrix;
	newWorldmat[3][0] = x;
	newWorldmat[3][1] = y;
	newWorldmat[3][2] = z;
	SetWorldMatrix(newWorldmat);
	return *this;
}

Transform & Transform::SetPosition(vec3 pos)
{
	SetPosition(pos.x, pos.y, pos.z);
	return *this;
}

Transform & Transform::AddWorldPosition(float dx, float dy, float dz)
{
	localPosition += dx * worldRightAxis;
	localPosition += dy * worldUpAxis;
	localPosition += dz * worldForwardAxis;

	UpdateTransform();
	return *this;
}

Transform & Transform::AddWorldPosition(vec3 delta)
{
	AddWorldPosition(delta.x, delta.y, delta.z);
	return *this;
}

Transform & Transform::SetLocalPosition(float x, float y, float z)
{
	localPosition.x = x;
	localPosition.y = y;
	localPosition.z = z;

	UpdateTransform();
	return *this;
}

Transform & Transform::SetLocalPosition(vec3 pos)
{
	localPosition = pos;

	UpdateTransform();
	return *this;
}

Transform & Transform::AddLocalPosition(float dx, float dy, float dz)
{
	localPosition += dx * right;
	localPosition += dy * up;
	localPosition += dz * forward;

	UpdateTransform();
	return *this;
}

Transform & Transform::AddLocalPosition(vec3 delta)
{
	AddLocalPosition(delta.x, delta.y, delta.z);

	return *this;
}

Transform & Transform::SetLocalScale(float x, float y, float z)
{
	localScale.x = x;
	localScale.y = y;
	localScale.z = z;

	UpdateTransform();
	return *this;
}

Transform & Transform::SetLocalScale(vec3 scale)
{
	localScale = scale;

	UpdateTransform();
	return *this;
}

Transform & Transform::SetRotateLocal(const quat & quaternion)
{
	SetRotateLocal(toMat4(quaternion));

	return *this;
}

Transform & Transform::SetRotateLocal(const mat4x4 & matRotation)
{
	right = normalize(vec3(
		matRotation[0][0],
		matRotation[0][1],
		matRotation[0][2]));
	

	up = normalize(vec3(
		matRotation[1][0],
		matRotation[1][1],
		matRotation[1][2]));

	forward = normalize(vec3(
		matRotation[1][0],
		matRotation[1][1],
		matRotation[1][2]));

	UpdateTransform();
	return *this;
}

Transform & Transform::SetRotateAxisX(const vec3 & axis)
{
	return *this;
}

Transform & Transform::SetRotateAxisY(const vec3 & axis)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::SetRotateAxisZ(const vec3 & axis)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisWorld(float axisXAngle, float axisYAngle, float axisZAngle)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisWorld(vec3 deltaAngle)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisLocal(float dAngleX, float dAngleY, float dAngleZ)
{
	//자신의 Right 축 기준으로 회전하는 회전 행렬을 얻는다.
	mat4 identity = mat4();

	mat4 matRotateX = rotate(identity, radians(dAngleX), this->right);

	//자신의 Up 축 기준으로 회전하는 회전 행렬을 얻는다.
	mat4 matRotateY = rotate(identity, radians(dAngleY), this->up);

	//자신의 Front 축 기준으로 회전하는 회전 행렬을 얻는다.
	mat4 matRotateZ = rotate(identity, radians(dAngleZ), this->forward);

	mat4 matRotate = matRotateZ * matRotateX * matRotateY;

	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++) 
	{
		vec4 axisVec4 = matRotate * vec4(axis[i], 1);
		axis[i] = vec3(axisVec4);
	}

	UpdateTransform();
	return *this;
}

Transform & Transform::RotateAxisLocal(vec3 deltaAngle)
{
	RotateAxisLocal(deltaAngle.x, deltaAngle.y, deltaAngle.z);
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::UpdateTransform()
{
	right = normalize(right);
	up = normalize(up);
	forward = normalize(forward);

	//Final 행렬
	vec3 r = this->right * this->localScale.x;
	vec3 u = this->up * this->localScale.y;
	vec3 f = this->forward * this->localScale.z;
	vec3 p = this->localPosition;

	//Final Matrix 에 적용
	localMatrix[0][0] = r.x;		localMatrix[1][0] = u.x;		localMatrix[2][0] = f.x;		localMatrix[3][0] = p.x;
	localMatrix[0][1] = r.y;		localMatrix[1][1] = u.y;		localMatrix[2][1] = f.y;		localMatrix[3][1] = p.y;
	localMatrix[0][2] = r.z;		localMatrix[1][2] = u.z;		localMatrix[2][2] = f.z;		localMatrix[3][2] = p.z;
	localMatrix[0][3] = 0;			localMatrix[1][3] = 0;			localMatrix[2][3] = 0;			localMatrix[3][3] = 1;

	if (parent != nullptr)
	{
		worldMatrix = parent->GetWorldMatrix() * localMatrix;
		worldPosition = vec3(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]);
		
		worldRightAxis = vec3(worldMatrix[0][0], worldMatrix[0][1], worldMatrix[0][2]);
		lossyScale.x = worldRightAxis.length();
		worldRightAxis = normalize(worldRightAxis);

		worldUpAxis = vec3(worldMatrix[1][0], worldMatrix[1][1], worldMatrix[1][2]);
		lossyScale.y = worldUpAxis.length();
		worldUpAxis = normalize(worldUpAxis);

		worldForwardAxis = vec3(worldMatrix[2][0], worldMatrix[2][1], worldMatrix[2][2]);
		lossyScale.z = worldForwardAxis.length();
		worldForwardAxis = normalize(worldForwardAxis);
	}
	else
	{
		worldMatrix = localMatrix;
		worldPosition = localPosition;
		lossyScale = localScale;
	}
	if (gameObject == nullptr)
		return *this;

	this->gameObject->GetCullSphere().center = worldPosition;
	Octree* octree = SceneGraph::GetInstance().octree;
	if (this->gameObject->GetOctreeLocCode())
	{
		this->gameObject->RefreshOctreeNode();
	}
	
	for (int i = 0; i < v_Children.size(); ++i)
		v_Children[i]->UpdateTransform();

	return *this;
}

vec3 Transform::GetWorldPosition() const
{
	return worldPosition;
}

vec3 Transform::GetLocalPosition() const
{
	return localPosition;
}

vec3 Transform::GetLocalScale() const
{
	return localScale;
}

vec3 Transform::GetLossyScale() const
{
	return lossyScale;
}

mat4x4 Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

mat4x4 Transform::GetLocalMatrix() const
{
	return localMatrix;
}

Transform & Transform::SetWorldMatrix(mat4x4 mat)
{
	mat4x4 newLocalMat;
	if(parent != nullptr)
		newLocalMat = inverse(parent->GetWorldMatrix()) * mat;
	else
		newLocalMat = mat;

	SetLocalMatrix(newLocalMat);

	return *this;
}

Transform & Transform::SetLocalMatrix(mat4x4 mat)
{
	up = vec3(mat[1][0], mat[1][1], mat[1][2]);
	right = vec3(mat[0][0], mat[0][1], mat[0][2]);
	forward = vec3(mat[2][0], mat[2][1], mat[2][2]);

	localScale = vec3(up.length(), right.length(), forward.length());

	localPosition = vec3(mat[3][0], mat[3][1], mat[3][2]);

	UpdateTransform();

	return *this;
}

vec3 Transform::GetBack(bool bNormalize) const
{
	return glm::normalize(glm::vec3(worldMatrix[2]));
}

vec3 Transform::GetUp(bool bNormalize) const
{
	return glm::normalize(glm::vec3(worldMatrix[1]));
}

vec3 Transform::GetRight(bool bNormalize) const
{
	return glm::normalize(glm::vec3(worldMatrix[0]));
}

Transform & Transform::SetParent(Transform * parent, bool attachWorld)
{
	if (parent == nullptr)
	{
		this->parent = nullptr;
		if (attachWorld)
			SetWorldMatrix(worldMatrix);
		else
			UpdateTransform();
	}
	else
		parent->AddChild(this, attachWorld);

	return *this;
}

Transform & Transform::AddChild(Transform * child, bool attachWorld)
{
	child->parent = this;
	v_Children.push_back(child);

	if (attachWorld)
		child->SetWorldMatrix(child->GetWorldMatrix());
	else
		child->UpdateTransform();

	return *this;
}

Transform * Transform::GetRoot()
{
	Scene* sceneGraphRoot = SceneGraph::GetInstance().scene;
	Transform* transform = this;
	while (transform->parent != sceneGraphRoot)
	{
		transform = transform->parent;
	}
	return transform;
}

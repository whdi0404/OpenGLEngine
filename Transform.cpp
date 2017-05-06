#include "stdafx.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "Scene.h"
#include "Octree.h"
#include "GameObject.h"

std::vector<Transform*> Transform::movedTransform;

Transform::Transform() : localPosition(0, 0, 0), localScale(1, 1, 1), worldMatrix(1.0f),
parent(nullptr), gameObject(nullptr), moved(false)
{
	this->forward = glm::vec3(0, 0, 1);
	this->up = glm::vec3(0, 1, 0);
	this->right = glm::vec3(1, 0, 0);

	localMatrix = worldMatrix = glm::mat4x4();
}


Transform::~Transform()
{
}

Transform & Transform::SetPosition(float x, float y, float z)
{
	glm::mat4x4 newWorldmat = worldMatrix;
	newWorldmat[3][0] = x;
	newWorldmat[3][1] = y;
	newWorldmat[3][2] = z;
	SetWorldMatrix(newWorldmat);
	return *this;
}

Transform & Transform::SetPosition(glm::vec3 pos)
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

Transform & Transform::AddWorldPosition(glm::vec3 delta)
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

Transform & Transform::SetLocalPosition(glm::vec3 pos)
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

Transform & Transform::AddLocalPosition(glm::vec3 delta)
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

Transform & Transform::SetLocalScale(glm::vec3 scale)
{
	localScale = scale;

	UpdateTransform();
	return *this;
}

Transform & Transform::SetRotateLocal(const glm::quat & quaternion)
{
	SetRotateLocal(toMat4(quaternion));

	return *this;
}

Transform & Transform::SetRotateLocal(const glm::mat4x4 & matRotation)
{
	right = normalize(glm::vec3(
		matRotation[0][0],
		matRotation[0][1],
		matRotation[0][2]));
	

	up = normalize(glm::vec3(
		matRotation[1][0],
		matRotation[1][1],
		matRotation[1][2]));

	forward = normalize(glm::vec3(
		matRotation[2][0],
		matRotation[2][1],
		matRotation[2][2]));

	UpdateTransform();
	return *this;
}

Transform & Transform::SetRotateAxisX(const glm::vec3 & axis)
{
	return *this;
}

Transform & Transform::SetRotateAxisY(const glm::vec3 & axis)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::SetRotateAxisZ(const glm::vec3 & axis)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisWorld(float axisXAngle, float axisYAngle, float axisZAngle)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisWorld(glm::vec3 deltaAngle)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

Transform & Transform::RotateAxisLocal(float dAngleX, float dAngleY, float dAngleZ)
{
	//자신의 Right 축 기준으로 회전하는 회전 행렬을 얻는다.
	glm::mat4x4 identity = glm::mat4x4();

	glm::mat4x4 matRotateX = glm::rotate(identity, glm::radians(dAngleX), this->right);

	//자신의 Up 축 기준으로 회전하는 회전 행렬을 얻는다.
	glm::mat4x4 matRotateY = glm::rotate(identity, glm::radians(dAngleY), this->up);

	//자신의 Front 축 기준으로 회전하는 회전 행렬을 얻는다.
	glm::mat4x4 matRotateZ = glm::rotate(identity, glm::radians(dAngleZ), this->forward);

	glm::mat4x4 matRotate = matRotateZ * matRotateX * matRotateY;

	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++) 
	{
		glm::vec4 axisVec4 = matRotate * glm::vec4(axis[i], 1);
		axis[i] = glm::vec3(axisVec4);
	}

	UpdateTransform();
	return *this;
}

Transform & Transform::RotateAxisLocal(glm::vec3 deltaAngle)
{
	RotateAxisLocal(deltaAngle.x, deltaAngle.y, deltaAngle.z);
	return *this;
}

Transform & Transform::UpdateTransform()
{
	right = normalize(right);
	up = normalize(up);
	forward = normalize(forward);

	//Final 행렬
	glm::vec3 r = this->right * this->localScale.x;
	glm::vec3 u = this->up * this->localScale.y;
	glm::vec3 f = this->forward * this->localScale.z;
	glm::vec3 p = this->localPosition;

	//Final Matrix 에 적용
	localMatrix[0][0] = r.x;		localMatrix[1][0] = u.x;		localMatrix[2][0] = f.x;		localMatrix[3][0] = p.x;
	localMatrix[0][1] = r.y;		localMatrix[1][1] = u.y;		localMatrix[2][1] = f.y;		localMatrix[3][1] = p.y;
	localMatrix[0][2] = r.z;		localMatrix[1][2] = u.z;		localMatrix[2][2] = f.z;		localMatrix[3][2] = p.z;
	localMatrix[0][3] = 0;			localMatrix[1][3] = 0;			localMatrix[2][3] = 0;			localMatrix[3][3] = 1;
	
	if (parent != nullptr)
	{
		worldMatrix = parent->GetWorldMatrix() * localMatrix;
		worldPosition = glm::vec3(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]);
		
		worldRightAxis = glm::vec3(worldMatrix[0][0], worldMatrix[0][1], worldMatrix[0][2]);
		lossyScale.x = glm::length(worldRightAxis);
		worldRightAxis = normalize(worldRightAxis);

		worldUpAxis = glm::vec3(worldMatrix[1][0], worldMatrix[1][1], worldMatrix[1][2]);
		lossyScale.y = glm::length(worldUpAxis);
		worldUpAxis = normalize(worldUpAxis);

		worldForwardAxis = glm::vec3(worldMatrix[2][0], worldMatrix[2][1], worldMatrix[2][2]);
		lossyScale.z = glm::length(worldForwardAxis);
		worldForwardAxis = normalize(worldForwardAxis);
	}
	else
	{
		worldMatrix = localMatrix;
		worldPosition = localPosition;
		lossyScale = localScale;
	}
	for (int i = 0; i < v_Children.size(); ++i)
		v_Children[i]->UpdateTransform();

	if (gameObject == nullptr)
		return *this;

	if (gameObject->renderObject != nullptr)
	{
		Math::Sphere*& rendererCullSphere = gameObject->renderObject->GetCullSphere();
		if (rendererCullSphere != nullptr)
		{
			glm::vec3 centerV3 = rendererCullSphere->center;
			glm::vec4 localCenter(centerV3.x, centerV3.y, centerV3.z, 1);
			glm::vec4 worldCenter = glm::mat4x4(worldMatrix) * localCenter;

			gameObject->cullSphere.center = glm::vec3(worldCenter.x, worldCenter.y, worldCenter.z);
			gameObject->cullSphere.radius = rendererCullSphere->radius * glm::max(this->GetLossyScale().x, this->GetLossyScale().y, this->GetLossyScale().z);
		}
		else
		{
			gameObject->cullSphere.center = worldPosition;
			gameObject->cullSphere.radius = 0;
		}
		if (moved == false)
		{
			movedTransform.push_back(this);
			moved = true;
		}
	}

	return *this;
}

glm::vec3 Transform::GetWorldPosition() const
{
	return worldPosition;
}

glm::vec3 Transform::GetLocalPosition() const
{
	return localPosition;
}

glm::vec3 Transform::GetLocalScale() const
{
	return localScale;
}

glm::vec3 Transform::GetLossyScale() const
{
	return lossyScale;
}

glm::mat4x4 Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

glm::mat4x4 Transform::GetLocalMatrix() const
{
	return localMatrix;
}

Transform & Transform::SetWorldMatrix(glm::mat4x4 mat)
{
	glm::mat4x4 newLocalMat;
	if(parent != nullptr)
		newLocalMat = inverse(parent->GetWorldMatrix()) * mat;
	else
		newLocalMat = mat;

	SetLocalMatrix(newLocalMat);

	return *this;
}

Transform & Transform::SetLocalMatrix(glm::mat4x4 mat)
{
	up = glm::vec3(mat[1][0], mat[1][1], mat[1][2]);
	right = glm::vec3(mat[0][0], mat[0][1], mat[0][2]);
	forward = glm::vec3(mat[2][0], mat[2][1], mat[2][2]);
	
	localScale = glm::vec3(glm::length(up), glm::length(right), glm::length(forward));
	localPosition = glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
	UpdateTransform();

	return *this;
}

glm::vec3 Transform::GetBack(bool bNormalize) const
{
	return glm::normalize(glm::vec3(worldMatrix[2]));
}

glm::vec3 Transform::GetUp(bool bNormalize) const
{
	return glm::normalize(glm::vec3(worldMatrix[1]));
}

glm::vec3 Transform::GetRight(bool bNormalize) const
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

glm::mat3x3 Transform::GetWorldRotateMatrix() const
{
	return glm::mat3x3(worldMatrix);
}

glm::mat3x3 Transform::GetLocalRotateMatrix() const
{
	return glm::mat3x3(localMatrix);
}

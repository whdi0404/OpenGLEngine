#pragma once
#include "Object.h"

using namespace glm;
class Transform : public Object
{
	friend class GameObject;
	friend class SceneGraph;
private:
	union {
		struct {
			vec3 right;
			vec3 up;
			vec3 forward;
		};

		struct {
			vec3 axis[3];
		};
	};

private:
	std::vector<Transform*> v_Children;
	Transform* parent;
	GetMacro(GameObject*, GameObject, gameObject);

private:
	vec3 worldRightAxis;
	vec3 worldUpAxis;
	vec3 worldForwardAxis;

	vec3 worldPosition;
	vec3 lossyScale;

	vec3 localPosition;
	vec3 localScale;

	mat4x4 worldMatrix;
	mat4x4 localMatrix;
public:
	Transform();
	virtual ~Transform();

public:

	//Translate
	Transform& SetPosition(float x, float y, float z);
	Transform& SetPosition(vec3 pos);

	Transform& AddWorldPosition(float dx, float dy, float dz);
	Transform& AddWorldPosition(vec3 delta);

	Transform& SetLocalPosition(float x, float y, float z);
	Transform& SetLocalPosition(vec3 pos);

	Transform& AddLocalPosition(float dx, float dy, float dz);
	Transform& AddLocalPosition(vec3 delta);

	//Scale
	Transform& SetLocalScale(float x, float y, float z);
	Transform& SetLocalScale(vec3 scale);

	//Rotate
	Transform& SetRotateLocal(const quat& quaternion);
	Transform& SetRotateLocal(const mat4x4& matRotation);

	Transform& SetRotateAxisX(const vec3& axis);
	Transform& SetRotateAxisY(const vec3& axis);
	Transform& SetRotateAxisZ(const vec3& axis);

	Transform& RotateAxisWorld(float axisXAngle, float axisYAngle, float axisZAngle);
	Transform& RotateAxisWorld(vec3 deltaAngle);

	Transform& RotateAxisLocal(float axisXAngle, float axisYAngle, float axisZAngle);
	Transform& RotateAxisLocal(vec3 deltaAngle);

	//특정 방향을 바라보게 회전해라.
	Transform& LookDirection(const vec3& dir, const vec3& Up = vec3(0, 1, 0));

	//특정방향을 바라보는데 angle 각만큼만 회전 해라
	Transform& LookDirection(const vec3& dir, float angle);

	//특정위치를 바라보게 회전해라.
	Transform& LookPosition(const vec3& pos, const vec3& Up = vec3(0, 1, 0));

	//특정위치를  바라보는데 angle 각만큼만 회전 해라
	Transform& LookPosition(const vec3& pos, float angle);

	//Transform 을 업데이트 한다 ( Trasform 의 정보가 갱신되었을때 사용된다 )
	Transform& UpdateTransform();


	// ------------------------------------------------------------------------
	// Get 관련
	// ------------------------------------------------------------------------

	//월드 위치를 얻는다.
	vec3 GetWorldPosition() const;
	vec3 GetLocalPosition() const;

	//크기를 얻는다.
	vec3 GetLocalScale()const;
	vec3 GetLossyScale()const;

	//최종행렬을 얻는다.
	mat4x4 GetWorldMatrix() const;
	mat4x4 GetLocalMatrix() const;
	Transform& SetWorldMatrix(mat4x4 mat);
	Transform& SetLocalMatrix(mat4x4 mat);

	//축을 얻는다. 
	vec3 GetBack(bool bNormalize = true) const;
	vec3 GetUp(bool bNormalize = true) const;
	vec3 GetRight(bool bNormalize = true) const;
	void GetScaledAxies(vec3* pVecArr) const;
	void GetUnitAxies(vec3* pVecArr) const;
	vec3 GetScaledAxis(int axisNum) const;
	vec3 GetUnitAxis(int axisNum) const;

	//월드 행렬에서 회전행렬 성분만 가져온다.
	mat4x4 GetWorldRotateMatrix() const;

	//월드사원수를 얻는다.
	quat GetWorldQuaternion() const;

	Transform& operator*=(const Transform& trans);
	Transform& operator*=(const mat4x4& matrix);

	//SetParent를 중점으로 구현
	Transform& SetParent(Transform* parent, bool attachWorld = true);
	Transform& AddChild(Transform* child, bool attachWorld = true);

	Transform* GetChild(int index) { return v_Children[index]; }
	size_t GetChildCount() { return v_Children.size(); }

	Transform* GetRoot();
};
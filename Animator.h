#pragma once
#include "Component.h"

class SkinnedMeshRenderObject;
class Animator :
	public Component
{
public:
	Animator();
	~Animator();

	void Update();

	GetSetMacro(float, NowTime, nowTime);
	GetMacro(KeyFrameAnimation*, NowAnimation, nowAnimation);
	GetSetMacro(Avatar*, Avatar, avatar);
public:
	void SetNowAnimation(KeyFrameAnimation* keyFrameAnim);
};


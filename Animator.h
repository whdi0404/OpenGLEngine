#pragma once
#include "Component.h"

class SkinnedMeshRenderObject;
class Animator :
	public Component
{
	GetSetMacro(float, NowTime, nowTime);
	GetMacro(KeyFrameAnimation*, NowAnimation, nowAnimation);
	GetSetMacro(Avatar*, Avatar, avatar);

public:
	Animator();
	~Animator();
	void Initialize() override;
	void Update() override;

public:
	void SetNowAnimation(KeyFrameAnimation* keyFrameAnim);
};


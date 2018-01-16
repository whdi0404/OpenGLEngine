#include "stdafx.h"
#include "Animator.h"
#include "SkinnedMeshRenderObject.h"
#include "Avatar.h"
#include "Time.h"

Animator::Animator()
{
	nowTime = 0;
}


Animator::~Animator()
{
}

void Animator::Update()
{
	float time = Time::GetInstance().GetDeltaTime();
	nowTime += time;
	while(nowTime >= nowAnimation->GetAnimationTime())
		nowTime -= nowAnimation->GetAnimationTime();
	avatar->UpdateAnimation(nowAnimation, nowTime);
}

void Animator::SetNowAnimation(KeyFrameAnimation * keyFrameAnim)
{
	nowAnimation = keyFrameAnim;
	nowTime = 0;
}



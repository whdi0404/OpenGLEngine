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
	double time = Time::GetInstance().GetDeltaTime();
	nowTime += time;
	if(nowTime > nowAnimation->GetAnimationTime())
		nowTime -= nowAnimation->GetAnimationTime();
	avatar->Update(nowAnimation, nowTime);
}

void Animator::SetNowAnimation(KeyFrameAnimation * keyFrameAnim)
{
	nowAnimation = keyFrameAnim;
	nowTime = 0;
}



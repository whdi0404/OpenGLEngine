#include "stdafx.h"
#include "KeyFrameAnimation.h"


void AnimationLayer::AddKeyFrame(int boneIndex, KeyFrame* keyFrame)
{
	KeyFrameTranslation* translation = dynamic_cast<KeyFrameTranslation*>(keyFrame);
	if (translation != nullptr)
	{
		if (boneIndex >= translateKeyFrames.size())
			translateKeyFrames.resize(boneIndex + 1);
		translateKeyFrames[boneIndex].push_back(translation);

		std::sort(translateKeyFrames[boneIndex].begin(), translateKeyFrames[boneIndex].end(), [](KeyFrameTranslation*& a, KeyFrameTranslation*& b)-> bool
		{
			return a->time < b->time;
		});
		return;
	}

	KeyFrameRotation* rotation = dynamic_cast<KeyFrameRotation*>(keyFrame);
	if (rotation != nullptr)
	{
		if (boneIndex >= rotateKeyFrames.size())
			rotateKeyFrames.resize(boneIndex + 1);
		rotateKeyFrames[boneIndex].push_back(rotation);
		std::sort(rotateKeyFrames[boneIndex].begin(), rotateKeyFrames[boneIndex].end(), [](KeyFrameRotation*& a, KeyFrameRotation*& b)-> bool
		{
			return a->time < b->time;
		});
		return;
	}

	KeyFrameScaling* scaling = dynamic_cast<KeyFrameScaling*>(keyFrame);
	if (scaling != nullptr)
	{
		if (boneIndex >= scaleKeyFrames.size())
			scaleKeyFrames.resize(boneIndex + 1);
		scaleKeyFrames[boneIndex].push_back(scaling);
		std::sort(scaleKeyFrames[boneIndex].begin(), scaleKeyFrames[boneIndex].end(), [](KeyFrameScaling*& a, KeyFrameScaling*& b)-> bool
		{
			return a->time < b->time;
		});
		return;
	}
}


KeyFrameAnimation::KeyFrameAnimation()
{
}


KeyFrameAnimation::~KeyFrameAnimation()
{
}

void KeyFrameAnimation::AddKeyFrame(int layerIndex, int boneIndex, KeyFrame * keyFrame)
{
	if (layerIndex >= animationLayers.size())
		animationLayers.resize(layerIndex + 1, nullptr);
	if (animationLayers[layerIndex] == nullptr)
		animationLayers[layerIndex] = new AnimationLayer();

	animationLayers[layerIndex]->AddKeyFrame(boneIndex, keyFrame);
}

glm::mat4x4 KeyFrameAnimation::GetMatrices(float time, int boneIndex)
{
	return mat4x4();
}
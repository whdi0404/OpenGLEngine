#include "stdafx.h"
#include "KeyFrameAnimation.h"

void AnimationLayer::AddKeyFrame(int boneIndex, KeyFrame* keyFrame)
{
	KeyFrameTranslation* translation = dynamic_cast<KeyFrameTranslation*>(keyFrame);
	if (translation != nullptr)
	{
		if (boneIndex >= translateKeyFrames.size())
			translateKeyFrames.resize(boneIndex + 1);
		translateKeyFrames[boneIndex].emplace_back(translation);

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
		rotateKeyFrames[boneIndex].emplace_back(rotation);
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
		scaleKeyFrames[boneIndex].emplace_back(scaling);
		std::sort(scaleKeyFrames[boneIndex].begin(), scaleKeyFrames[boneIndex].end(), [](KeyFrameScaling*& a, KeyFrameScaling*& b)-> bool
		{
			return a->time < b->time;
		});
		return;
	}
}

glm::mat4x4 AnimationLayer::GetMatrices(int boneIndex, float time)
{
	std::vector<KeyFrameScaling*>& scaling = scaleKeyFrames[boneIndex];
	std::vector<KeyFrameRotation*>& rotation = rotateKeyFrames[boneIndex];
	std::vector<KeyFrameTranslation*>& translation = translateKeyFrames[boneIndex];

	glm::vec3 translate;
	glm::quat rotate;
	glm::vec3 scale;

	{
		int low = 0;
		int high = translation.size() - 1;
		int mid = 0;
		while (low < high) {
			mid = (low + high) / 2;
			if (translation[mid]->time > time)
				high = mid - 1;
			else if (translation[mid]->time < time)
				low = mid + 1;
			else
				break;
		}

		if (translation[mid]->time > time)
		{
			int start = glm::clamp(mid - 1, 0, (int)translation.size() - 1);
			int next = glm::clamp(mid, 0, (int)translation.size() - 1);
			float normalizedTime = inverseLerp(translation[start]->time, translation[next]->time, time);
			translate = lerp(translation[start]->value, translation[next]->value, normalizedTime);
		}
		else
		{
			int start = glm::clamp(mid, 0, (int)rotation.size() - 1);
			int next = (mid + 1) % translation.size();
			float normalizedTime = inverseLerp(translation[start]->time, translation[next]->time, time);
			translate = lerp(translation[start]->value, translation[next]->value, normalizedTime);
		}
	}
	{
		int low = 0;
		int high = rotation.size() - 1;
		int mid = 0;
		while (low < high) {
			mid = (low + high) / 2;
			if (rotation[mid]->time > time)
				high = mid - 1;
			else if (rotation[mid]->time < time)
				low = mid + 1;
			else
				break;
		}

		if (rotation[mid]->time > time)
		{
			int start = glm::clamp(mid - 1, 0, (int)rotation.size() - 1);
			int next = glm::clamp(mid, 0, (int)rotation.size() - 1);
			float normalizedTime = inverseLerp(rotation[start]->time, rotation[next]->time, time);
			rotate = glm::slerp(rotation[start]->value, rotation[mid]->value, normalizedTime);
		}
		else
		{
			int start = glm::clamp(mid, 0, (int)rotation.size() - 1);
			int next = (mid + 1) % rotation.size();
			float normalizedTime = inverseLerp(rotation[start]->time, rotation[next]->time, time);
			rotate = glm::slerp(rotation[start]->value, rotation[next]->value, normalizedTime);
		}
	}
	{
		int low = 0;
		int high = scaling.size() - 1;
		int mid = 0;
		while (low < high) {
			mid = (low + high) / 2;
			if (scaling[mid]->time > time)
				high = mid - 1;
			else if (scaling[mid]->time < time)
				low = mid + 1;
			else
				break;
		}

		if (scaling[mid]->time > time)
		{
			int start = glm::clamp(mid - 1, 0, (int)scaling.size() - 1);
			int next = glm::clamp(mid, 0, (int)scaling.size() - 1);
			float normalizedTime = inverseLerp(scaling[start]->time, scaling[next]->time, time);
			scale = lerp(scaling[mid - 1]->value, scaling[mid]->value, normalizedTime);
		}
		else
		{
			int start = glm::clamp(mid, 0, (int)scaling.size() - 1);
			int next = (mid + 1) % scaling.size();
			float normalizedTime = inverseLerp(scaling[start]->time, scaling[next]->time, time);
			scale = lerp(scaling[start]->value, scaling[next]->value, normalizedTime);
		}
	}
	//translate = translation[0]->value;
	//rotate = rotation[0]->value;
	//scale = scaling[0]->value;
	glm::mat4x4 s,t,tmp;
	tmp = glm::translate(t, translate) * glm::toMat4(rotate) * glm::scale(s, scale);
	return tmp;
}

KeyFrameAnimation::KeyFrameAnimation()
{
	animationTime = 0;
}

KeyFrameAnimation::~KeyFrameAnimation()
{
}

void KeyFrameAnimation::AddKeyFrame(int layerIndex, int boneIndex, KeyFrame * keyFrame)
{
	animationTime = glm::max(animationTime, keyFrame->time);

	if (layerIndex >= animationLayers.size())
		animationLayers.resize(layerIndex + 1, nullptr);
	if (animationLayers[layerIndex] == nullptr)
		animationLayers[layerIndex] = new AnimationLayer();

	animationLayers[layerIndex]->AddKeyFrame(boneIndex, keyFrame);
}

glm::mat4x4 KeyFrameAnimation::GetMatrices(int layerIndex, int boneIndex, float time)
{
	return animationLayers[layerIndex]->GetMatrices(boneIndex, time);
}
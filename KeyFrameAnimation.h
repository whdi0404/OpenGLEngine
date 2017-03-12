#pragma once
#include "Object.h"

struct KeyFrame
{
	KeyFrame() {}
	virtual ~KeyFrame() {}

	float time;
};

template <typename T>
struct KeyFrameTemplate : public KeyFrame
{
	KeyFrameTemplate() {}
	virtual ~KeyFrameTemplate() {}
	T value;
};

struct KeyFrameScaling : public KeyFrameTemplate<glm::vec3>
{
	KeyFrameScaling(glm::vec3 value, float time){
		this->value = value;
		this->time = time;
	}
};
struct KeyFrameTranslation : public KeyFrameTemplate<glm::vec3>
{
	KeyFrameTranslation(glm::vec3 value, float time) {
		this->value = value;
		this->time = time;
	}
};
struct KeyFrameRotation : public KeyFrameTemplate<glm::quat>
{
	KeyFrameRotation(glm::quat value, float time) {
		this->value = value;
		this->time = time;
	}
};

struct AnimationLayer
{
	std::vector<std::vector<KeyFrameScaling*>> scaleKeyFrames;
	std::vector<std::vector<KeyFrameRotation*>> rotateKeyFrames;
	std::vector<std::vector<KeyFrameTranslation*>> translateKeyFrames;

	void AddKeyFrame(int boneIndex, KeyFrame* keyFrame);
};

class KeyFrameAnimation :
	public Object
{
	std::vector<AnimationLayer*> animationLayers;
public:
	KeyFrameAnimation();
	~KeyFrameAnimation();

	void AddKeyFrame(int layerIndex, int boneIndex, KeyFrame* keyFrame);

	glm::mat4x4 GetMatrices(float time, int boneIndex);
};
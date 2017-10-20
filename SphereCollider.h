#pragma once
#include "Collider.h"

class Camera;
class TerrainSystem;
class SphereCollider : public Collider
{
private:
	TerrainSystem* terrainSystem;
	Camera* camera;

public:
	enum
	{
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};
	SphereCollider() {}
	~SphereCollider() {}

	virtual void Initialize() override;

	void SetCamera(Camera* camera) { this->camera = camera; }
	virtual void OnTriggerEnter(Collider * collider) override;
	virtual void OnTriggerExit(Collider * collider) override;
};
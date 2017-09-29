#pragma once
#include "Collider.h"

class TerrainSystem;
class TerrainCollider : public Collider
{
	enum
	{
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};
private:
	TerrainSystem* terrain;

public:
	TerrainCollider() {}
	~TerrainCollider() {}

	virtual void OnTriggerEnter(Collider * collider) override;
	virtual void OnTriggerExit(Collider * collider) override;

	virtual void Initialize() override;
	virtual void Update() override {}

	void CreateCollider();

};
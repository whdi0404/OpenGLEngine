#pragma once
#include "Collider.h"

class TerrainSystem;
class TerrainCollider : public Collider
{
private:
	TerrainSystem* terrain;

	std::vector<PxRigidActor*> heightColliders;
	int clusterCellCount;
	int clusterScale;
	int tessellationFactor;

public:
	TerrainCollider() {}
	~TerrainCollider() {}

	virtual void OnTriggerEnter(Collider * collider) override;
	virtual void OnTriggerExit(Collider * collider) override;

	virtual void Initialize() override;
	virtual void Update() override;

	void CreateCollider();

};
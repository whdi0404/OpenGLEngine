#pragma once
#include "PracticalRenderObject.h"

class Texture2D;
class TerrainSystem :
	public PracticalRenderObject
{
	GetSetMacro(Camera*, Camera, camera);
private:
	Mesh* terrainMesh;

	GetMacro(glm::ivec2, TerrainSize, terrainSize);
	Texture2D* texture;

public:
	TerrainSystem();
	~TerrainSystem();

public:
	void CreateMesh(Texture2D* texture, float tileSize, float maxHeight);
	float GetHeight(glm::ivec2 pos);

	GetMacro(float, MaxHeight, maxHeight);

protected:
	virtual void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};
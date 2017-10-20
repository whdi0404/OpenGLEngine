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
	GetMacro(float, TileSize, tileSize);
	GetMacro(float, HeightScale, heightScale);
	Texture2D* texture;

public:
	TerrainSystem();
	~TerrainSystem();

public:
	void CreateMesh(Texture2D* texture, float tileSize, float heightScale);
	float GetHeight(glm::ivec2 pos);

protected:
	virtual void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};
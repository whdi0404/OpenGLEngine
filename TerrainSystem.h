#pragma once
#include "PracticalRenderObject.h"

class Texture2D;
class TerrainSystem :
	public PracticalRenderObject
{
public:
	TerrainSystem();
	~TerrainSystem();

private:
	Mesh* terrainMesh;
public:
	void CreateMesh(Texture2D* texture, float tileSize, float maxHeight);

protected:
	virtual void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};
#include "stdafx.h"
#include "TerrainSystem.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "MeshVertexAttribute.h"

TerrainSystem::TerrainSystem()
{
}

TerrainSystem::~TerrainSystem()
{
}
void TerrainSystem::CreateMesh(Texture2D * texture, float tileSize, float maxHeight)
{
	//텍스쳐 크기보다 셀이 더 세밀할 경우 등에 대비해 리니어 필터로 텍스쳐 불러와야 함
	terrainMesh = new Mesh();

	int texW = texture->GetWidth();
	int texH = texture->GetHeight();
	int width = texW / 32;
	int height = texH / 32;

	float widthDivTexW = (float)texW / (float)width;
	float heightDivTexH = (float)texH / (float)height;

	float mapHalfSize = width * 0.5f * tileSize;

	VertexBuffer* newVertexBuffer = new VertexBuffer(new MeshVertexAttribute(Element::Position | Element::Texcoord0));
	newVertexBuffer->SetVertexCount(width * height);
	std::vector<int> indices;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int arrayIdx = y * width + x;
			vec3 pos = vec3(x * tileSize - mapHalfSize, texture->GetPixel(min((float)x, (float)width), min((float)y, (float)height)).r * maxHeight /*- maxHeight * 0.5f*/, y * tileSize - mapHalfSize);

			newVertexBuffer->SetVector(Element::Position, arrayIdx, glm::vec4(pos.x, pos.y, pos.z, 0));
			newVertexBuffer->SetVector(Element::Texcoord0, arrayIdx, glm::vec4(float(y) / height, float(x) / width, 0, 0));
		}
	}

	for (int y = 0; y < height - 1; ++y)
	{
		for (int x = 0; x < width - 1; ++x)
		{
			indices.push_back((y + 1) * width + x);
			indices.push_back(y * width + x);

			indices.push_back(y * width + x + 1);
			indices.push_back((y + 1) * width + x + 1);

		}
	}
	terrainMesh->SetMeshData(newVertexBuffer, indices);

	SetMesh(terrainMesh);
}

void TerrainSystem::Render(std::vector<PracticalRenderObject*>& renderObjects, Camera *& camera)
{
	for (int i = 0; i < renderObjects.size(); ++i)
	{
		g_Renderer->matrixBuffer.push_back(renderObjects[i]->GetTransform()->GetWorldMatrix());
	}
	Material* mtrl = GetMaterial();
	if (g_Renderer->prevMtrl != mtrl)
	{
		g_Renderer->prevMtrl = mtrl;
		mtrl->BindShader();
		mtrl->SetCameraMatrix(camera);
	}
	mtrl->BindUniformValue();

	//GLint MaxPatchVertices = 0;
	//glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	//printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	GetMesh()->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size(), GL_PATCHES);
}
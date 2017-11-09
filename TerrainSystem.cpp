#include "stdafx.h"
#include "TerrainSystem.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "MeshVertexAttribute.h"
#include "Gizmo.h"
#include "math.h"

#define Tessellation

TerrainSystem::TerrainSystem()
{
}

TerrainSystem::~TerrainSystem()
{
}
void TerrainSystem::CreateMesh(Texture2D * texture, float tileScale, float heightScale)
{
	this->heightScale = heightScale;
	this->tileScale = tileScale;
	this->texture = texture;
	//텍스쳐 크기보다 셀이 더 세밀할 경우 등에 대비해 리니어 필터로 텍스쳐 불러와야 함
	terrainMesh = new Mesh();

	int texW = texture->GetWidth();
	int texH = texture->GetHeight();

	terrainSize = glm::ivec2(texW, texH);

	float tessellateCount = 1;
	int width = texW / tessellateCount + 1;
	int height = texH / tessellateCount + 1;

	float mapHalfSize = width * 0.5f * tileScale;

	VertexBuffer* newVertexBuffer = new VertexBuffer(TestVertexAttribute);
	newVertexBuffer->SetVertexCount(width * height);
	std::vector<int> indices;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int arrayIdx = y * width + x;
			glm::vec3 pos = glm::vec3(x * tileScale * tessellateCount, GetHeight(glm::vec2((float)x / width, (float)y / height)), y * tileScale * tessellateCount);

			newVertexBuffer->SetVector(Element::Position, arrayIdx, glm::vec4(pos.x, pos.y, pos.z, 0));
			newVertexBuffer->SetVector(Element::Texcoord0, arrayIdx, glm::vec4(float(x + 1) / (width), float(y + 1) / (height), 0, 0));
		}
	}

#ifdef Tessellation
	terrainMesh->SetDrawMode(GL_PATCHES);

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
	terrainMesh->SetDrawMode(GL_PATCHES);
#else
	for (int y = 0; y < height - 1; ++y)
	{
		for (int x = 0; x < width - 1; ++x)
		{
			indices.push_back((y + 1) * width + x);
			indices.push_back((y + 1) * width + x + 1);
			indices.push_back(y * width + x + 1);

			indices.push_back((y + 1) * width + x);
			indices.push_back(y * width + x + 1);
			indices.push_back(y * width + x);
		}
	}
#endif
	terrainMesh->SetMeshData(newVertexBuffer, indices);

	auto& sphere = terrainMesh->GetBoundingSphere();

	GetMaterial()->SetFloat(std::string("heightScale"), &heightScale, 1);
	GetMaterial()->SetFloat(std::string("tessellationCount"), &tessellateCount, 1);
	SetMesh(terrainMesh);

	GetTransform()->SetLocalPosition(0, 0, 0);
}

float TerrainSystem::GetHeight(glm::vec2 uv)
{
	return texture->GetPixelBilinear(uv).r * heightScale;
}

void TerrainSystem::Render(std::vector<PracticalRenderObject*>& renderObjects, Camera *& camera)
{
	for (int i = 0; i < renderObjects.size(); ++i)
	{
		g_Renderer->matrixBuffer.emplace_back(renderObjects[i]->GetTransform()->GetWorldMatrix());
	}
	Material* mtrl = GetMaterial();
	if (g_Renderer->prevMtrl != mtrl)
	{
		g_Renderer->prevMtrl = mtrl;
		mtrl->BindShader();
		mtrl->SetCameraMatrix(camera);
	}
	mtrl->BindUniformValue();

#ifdef Tessellation

	//GLint MaxPatchVertices = 0;
	//glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	//printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
#endif
	mesh->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size());
}
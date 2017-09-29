#include "stdafx.h"
#include "TerrainSystem.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "MeshVertexAttribute.h"
#include "Gizmo.h"
#include "math.h"

TerrainSystem::TerrainSystem()
{
}

TerrainSystem::~TerrainSystem()
{
}
void TerrainSystem::CreateMesh(Texture2D * texture, float tileSize, float maxHeight)
{
	this->texture = texture;
	this->maxHeight = maxHeight;
	//텍스쳐 크기보다 셀이 더 세밀할 경우 등에 대비해 리니어 필터로 텍스쳐 불러와야 함
	terrainMesh = new Mesh();

	int texW = texture->GetWidth();
	int texH = texture->GetHeight();
	terrainSize = glm::ivec2(texW, texH);

	const int tessellateCount = 32;
	int width = texW / tessellateCount;
	int height = texH / tessellateCount;

	//float widthDivTexW = (float)texW / (float)width;
	//float heightDivTexH = (float)texH / (float)height;

	float mapHalfSize = width * 0.5f * tileSize;

	VertexBuffer* newVertexBuffer = new VertexBuffer(TestVertexAttribute);
	newVertexBuffer->SetVertexCount(width * height);
	std::vector<int> indices;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int arrayIdx = y * width + x;
			glm::vec3 pos = glm::vec3(x * tileSize * tessellateCount /*- mapHalfSize*/, texture->GetPixel(glm::min((float)x, (float)width), glm::min((float)y, (float)height)).r * maxHeight /*- maxHeight * 0.5f*/, y * tileSize * tessellateCount /* - mapHalfSize*/);

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
	terrainMesh->SetDrawMode(GL_PATCHES);
	auto& sphere = terrainMesh->GetBoundingSphere();

	GetMaterial()->SetFloat(std::string("heightScale"), &maxHeight, 1);
	SetMesh(terrainMesh);

	GetTransform()->SetLocalPosition(0, 0, 0);
}

float TerrainSystem::GetHeight(glm::ivec2 pos)
{
	return texture->GetPixel(pos.x, pos.y).r * maxHeight;
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
	GetMesh()->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size());
}
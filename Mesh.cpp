#include "stdafx.h"
#include "Mesh.h"
#include "AttributeBase.h"
#include "Texture.h"
#include "GameObject.h"
#include "RenderObject.h"

unsigned short Mesh::totalResourceID = 0;

Mesh::Mesh()/* : attr(VertexAttrib::GetInstancePtr()), texture(new Texture2D("./Tex/Tex_0049_7.jpg"))*/
{
	resourceID = totalResourceID++;
}
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayID);
}
void Mesh::SetVertices(std::vector<DefaultVertex>& vertices, std::vector<int>& indices)
{
	this->vertices.clear();
	this->indices.clear();

	this->vertices = vertices;
	this->indices = indices;	

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(*this->vertices.data()) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(this->vertices[0]), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(this->vertices[0]), (GLvoid*)(sizeof(vec3)));

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*this->indices.data()) * this->indices.size(), this->indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &matrixBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 0));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 1));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 2));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 3));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}
void Mesh::DrawInstance(mat4x4* pMat, int count, GLenum drawMode)
{
	//glm::mat4 Proj = glm::perspective(glm::radians(45.0f), (float)1024.0f / (float)768.0f, 0.1f, 5000.0f);
	//glm::mat4 View = glm::inverse(camTrans.GetWorldMatrix());

	////BindShaderProgram
	//glUseProgram(shaderProgamID);
	//

	////BindView,Proj Matrices
	//GLuint matProj = glGetUniformLocation(shaderProgramID, "matProj");
	//glUniformMatrix4fv(matProj, 1, GL_FALSE, &Proj[0][0]);

	//GLuint matView = glGetUniformLocation(shaderProgramID, "matView");
	//glUniformMatrix4fv(matView, 1, GL_FALSE, &View[0][0]);

	glBindVertexArray(vertexArrayID);

	//BindMatrix
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * count, pMat, GL_STATIC_DRAW);

	//Texture Bind
	/*GLuint ShaderTexID = glGetUniformLocation(shaderProgramID, "myTextureSampler");
	glUniform1i(ShaderTexID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glBindSampler(ShaderTexID, texture->GetSamplerID());*/

	//Draw
	glDrawElementsInstanced(
		drawMode,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0,           // element array buffer offset
		count
	);

	glBindVertexArray(0);
}

std::vector<Mesh*> Mesh::LoadFBX(std::string filePath)
{
	if (g_FbxSdkManager == nullptr)
	{
		g_FbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_FbxSdkManager, IOSROOT);
		g_FbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_FbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_FbxSdkManager, "");

	bool bSuccess = pImporter->Initialize(filePath.c_str(), -1, g_FbxSdkManager->GetIOSettings());
	if (!bSuccess) return std::vector<Mesh*>();

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return std::vector<Mesh*>();

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	std::vector<vec3> position;
	std::vector<int> indices;
	std::vector<vec2> uvs;

	std::vector<Mesh*> Meshes;

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			position.clear();
			indices.clear();
			uvs.clear();

			Mesh* mesh = new Mesh();
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			FbxVector4* pVertices = pMesh->GetControlPoints();
			FbxLayerElementArrayTemplate<FbxVector2>* pTexUvs;
			pMesh->GetTextureUV(&pTexUvs);
			int vertexCount = pMesh->GetControlPointsCount();
			int count = pTexUvs->GetCount();
			//pOutPosition->resize(vertexCount);
			for (int j = 0; j < vertexCount; ++j)
			{
				vec3 vertex;
				vertex.x = (float)pVertices[j].mData[0];
				vertex.y = (float)pVertices[j].mData[1];
				vertex.z = (float)pVertices[j].mData[2];
				position.push_back(vertex);
				//(*pOutPosition)[j] = vertex;

				vec2 uv;
				uv.x = (*pTexUvs)[j].mData[0];
				uv.y = (*pTexUvs)[j].mData[1];
				uvs.push_back(uv);
			}

			int polygonCount = pMesh->GetPolygonCount();

			//pOutindices->resize(polygonCount * 3);
			for (int j = 0; j < polygonCount; j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);

				for (int k = 0; k < iNumVertices; k++)
				{
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					indices.push_back(iControlPointIndex);
					//(*pOutindices)[(j * 3) + k] = iControlPointIndex;
				}
			}

			std::vector<DefaultVertex> vertices;
			int size = position.size();
			for (int i = 0; i < size; ++i)
			{
				DefaultVertex vertex(position[i],uvs[i]);
				vertices.push_back(vertex);
			}

			mesh->SetVertices(vertices, indices);
			Meshes.push_back(mesh);
		}
	}

	return Meshes;
}
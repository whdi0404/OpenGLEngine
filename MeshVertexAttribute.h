#pragma once
//Position, TexCoord0~7, Color0~3, Normal, Tangent, weight0~4
enum Element {
	Position = 1 << 0,//Single3
	Normal = 1 << 1,//Single3
	Tangent = 1 << 2,//Single3
	Texcoord0 = 1 << 3,//Single2
	Texcoord1 = 1 << 4,//Single2
	Texcoord2 = 1 << 5,//Single2
	Texcoord3_Single4 = 1 << 6,//Single4
	Texcoord4_Single4 = 1 << 7,//Single4
	Texcoord5_Single4 = 1 << 8,//Single4
	Color0 = 1 << 9,//Single4
	Color1 = 1 << 10,//Single4
	Color2 = 1 << 11,//Single4
	Color3 = 1 << 12,//Single4
	BoneWeights = 1 << 13,//Single4
	BoneIndices = 1 << 14,//Single4
	End
};

class MeshVertexAttribute
{
	GetMacro(UINT16, AttributeFlag, attributeFlag);
	GetMacro(int, VertexSize, vertexSize);
	GetMacro(int, ElementCount, elementCount);

public:
	

	MeshVertexAttribute(UINT16 flag) : attributeFlag(flag), vertexSize(0), elementCount(0)
	{
		for (UINT16 i = 1; i < Element::End; i <<= 1)
			if (attributeFlag & i)
			{
				vertexSize += GetElementSize((Element)i);
				++elementCount;
			}
	}

	int GetElementSize(Element element)
	{
		if (attributeFlag & element)
		{
			switch (element)
			{
			case Element::Position:
			case Element::Normal:
			case Element::Tangent:
				return sizeof(glm::vec3);
			case Element::Texcoord0:
			case Element::Texcoord1:
			case Element::Texcoord2:
				return sizeof(glm::vec2);
			default:
				return sizeof(glm::vec4);
			}
		}
		return -1;
	}

	int GetElementStartOffset(Element element)
	{
		if (attributeFlag & element)
		{
			int startOffset = 0;
			for (UINT16 i = 1; i < element; i <<= 1)
				if (attributeFlag & i)
					startOffset += GetElementSize((Element)i);
			return startOffset;
		}
		return -1;		
	}

	bool HasElement(Element element)
	{
		return attributeFlag & element;
	}

	void EnableGLVertexAttributePointer()
	{
		for (UINT16 i = 1, arrayIndex = 0; i < Element::End; i <<= 1)
		{
			if (attributeFlag & i)
			{
				int floatCount = GetElementSize((Element)i) / sizeof(float);
				int startOffset = GetElementStartOffset((Element)i);
				glEnableVertexAttribArray(arrayIndex);
				glVertexAttribPointer(arrayIndex++, floatCount, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)startOffset);
			}
		}
	}
};
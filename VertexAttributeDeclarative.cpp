#include "stdafx.h"
#include "VertexAttributeDeclarative.h"
#include "MeshVertexAttribute.h"
MeshVertexAttribute* TestVertexAttribute = new MeshVertexAttribute(Element::Position | Element::Texcoord0);
MeshVertexAttribute* SkinningVertexAttribute = new MeshVertexAttribute(Element::Position | Element::Texcoord0 | Element::BoneWeights | Element::BoneIndices);
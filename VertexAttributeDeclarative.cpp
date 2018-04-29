#include "stdafx.h"
#include "VertexAttributeDeclarative.h"
#include "MeshVertexAttribute.h"
MeshVertexAttribute* TestVertexAttribute = new MeshVertexAttribute(Element::Position | Element::Texcoord0 | Element::Normal | Element::Tangent);
MeshVertexAttribute* SkinningVertexAttribute = new MeshVertexAttribute(Element::Position | Element::Texcoord0 | Element::Normal | Element::Tangent | Element::BoneWeights | Element::BoneIndices);
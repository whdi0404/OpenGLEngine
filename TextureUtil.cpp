#include "stdafx.h"
#include "TextureUtil.h"

using namespace TextureUtil;

Color TextureUtil::bilinear(const float & tx, const float & ty, const Color & c00, const Color & c10, const Color & c01, const Color & c11)
{
	glm::vec4 colVec = 
		(1.0f - tx) * (1.0f - ty) * glm::vec4(c00.r, c00.g, c00.b, c00.a) +
		tx * (1.0f - ty) * glm::vec4(c10.r, c10.g, c10.b, c10.a) +
		(1.0f - tx) * ty * glm::vec4(c01.r, c01.g, c01.b, c01.a) +
		tx * ty * glm::vec4(c11.r, c11.g, c11.b, c11.a);

	return Color(glm::floor(colVec.r * 255) / 255.0f, glm::floor(colVec.g * 255) / 255.0f, glm::floor(colVec.b * 255) / 255.0f, glm::floor(colVec.a * 255) / 255.0f);
}

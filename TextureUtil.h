#pragma once

namespace TextureUtil
{
	class Color
	{
	public:
		Color() : r(0), g(0), b(0), a(0)
		{
		}
		Color(float r, float g, float b, float a = 1.0f)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
	public:
		float r, g, b, a;
	};

	Color bilinear(
		const float &tx,
		const float  &ty,
		const Color &c00,
		const Color &c10,
		const Color &c01,
		const Color &c11);
}
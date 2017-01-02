#pragma once

#include "Object.h"

class Color
{
public:
	Color() : r(0),g(0),b(0),a(0)
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

class Texture2D : public Object
{
public:
	enum Format
	{
		ColorIndex = IL_COLOR_INDEX,
		Alpha = IL_ALPHA,
		RGB = IL_RGB,
		RGBA = IL_RGBA,
		BGR = IL_BGR,
		BGRA = IL_BGRA,
		Luminance = IL_LUMINANCE,
		LuminanceAlpha = IL_LUMINANCE_ALPHA
	};
	enum DataType
	{
		Byte = IL_BYTE,
		Unsigned_Byte = IL_UNSIGNED_BYTE,
		Short = IL_SHORT,
		Unsigned_Short = IL_UNSIGNED_SHORT,
		Int = IL_INT,
		Unsigned_Int = IL_UNSIGNED_INT,
		Float = IL_FLOAT,
		Double = IL_DOUBLE,
		Half = IL_HALF
	};

	Get(GLuint, TextureID, textureID)
	Get(unsigned int, Width, width)
	Get(unsigned int, Height, height)
	Get(size_t, BytePerPixel, bytePerPixel)
	Get(Format, Format, format)
	Get(DataType, DataType, dataType)
	Get(GLuint, SamplerID, samplerID)
	Get(ILuint, ILTextureID, ilTextureID)

	Color GetPixel(int x, int y);

public:
	Texture2D(std::string filename);
	~Texture2D();
};
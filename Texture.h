#pragma once

#include "Object.h"
#include "TextureUtil.h"

using namespace TextureUtil;
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

public:
	Texture2D(std::string filename);
	~Texture2D();

	GetMacro(GLuint, TextureID, textureID)
	GetMacro(int, Width, width)
	GetMacro(int, Height, height)
	GetMacro(size_t, BytePerPixel, bytePerPixel)
	GetMacro(Format, Format, format)
	GetMacro(DataType, DataType, dataType)
	GetMacro(GLuint, SamplerID, samplerID)
	GetMacro(ILuint, ILTextureID, ilTextureID)

	GetSetMacro(GLuint, MagFilter, magFilter)
	GetSetMacro(GLuint, MinFilter, minFilter)
	GetSetMacro(GLuint, Aniso, aniso)

	Color GetPixel(int x, int y);
	Color GetPixelBilinear(glm::vec2 uv);
};
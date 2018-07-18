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
	Texture2D(int width, int height, DataType dataType, Format format);
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

	void SetPixel(int x, int y, Color color);

	void ApplyChange();
	static Texture2D* CreateRandomHeightmap(int width, int height, int fractal, float h, Texture2D* heightSeed, Texture2D* surfaceSeed, bool isFractal);

	static Texture2D* CreateNormalTexture(Texture2D* heightmap, int width, int height, float tileSize, float scale);

	static Texture2D* ResizeTexture(Texture2D* heightmap, int width, int height);

	static size_t FormatElemCountPerPixel(Format format)
	{
		switch (format)
		{
		case ColorIndex:
		case Alpha:
		case Luminance://확실하지 않음
			return 1;
		case LuminanceAlpha://확실하지 않음
			return 2;
		case RGB:
		case BGR:
			return 3;
		case RGBA:
		case BGRA:
			return 4;
		}
	}

	static size_t DataTypeBytePerPixel(DataType dataType)
	{
		switch (dataType)
		{
		case Byte:
		case Unsigned_Byte:
			return 1;
		case Half:
		case Short:
		case Unsigned_Short:
			return 2;
		case Int:
		case Unsigned_Int:
		case Float:
			return 4;
		case Double:
			return 8;
		}
	}

	size_t GetBytePerPixel(DataType dataType, Format format)
	{
		return FormatElemCountPerPixel(format) * DataTypeBytePerPixel(dataType);
	}

	void* GetData();

};
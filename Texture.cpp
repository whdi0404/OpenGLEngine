#include "stdafx.h"
#include "Texture.h"

Texture2D::Texture2D(std::string filename) :
	width(0), height(0), bytePerPixel(0)
{
	ilTextureID = -1;
	// generate an image name
	ilGenImages(1, &ilTextureID);
	// bind it
	ilBindImage(ilTextureID);
	// match image origin to OpenGL’s
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	// load  the image
	bool success = ilLoadImage((ILstring)filename.c_str());
	// check to see if everything went OK
	if (!success) {
		ilDeleteImages(1, &ilTextureID);
		ilTextureID = -1;
		return;
	}

	//TODO: 원하는 텍스쳐 포멧으로 컨버트 하도록
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bytePerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	format = (Format)ilGetInteger(IL_IMAGE_FORMAT);
	dataType = (DataType)ilGetInteger(IL_IMAGE_TYPE);

	/* Create and load textures to OpenGL */
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format,
		width,
		height,
		0, format, dataType,
		ilGetData());

	glGenSamplers(1, &samplerID);

	//GL_LINEAR_MIPMAP_LINEAR;
	magFilter = GL_LINEAR;
	minFilter = GL_LINEAR;
	aniso = 1;
}

Texture2D::~Texture2D()
{
	ilDeleteImage(ilTextureID);
	glDeleteTextures(1, &textureID);
	glDeleteSamplers(1, &samplerID);
}

Color Texture2D::GetPixelBilinear(glm::vec2 uv)
{
	uv = glm::saturate(uv);

	////uv += glm::vec2(1.0f / width, 1.0f / height) * 0.5f;
	//
	//float invWidth = 1.0f / width;
	//float invHeight = 1.0f / height;

	//float tx = (uv.x - ((int)(uv.x / invWidth) * invWidth)) / invWidth;
	//float ty = (uv.y - ((int)(uv.y / invHeight) * invHeight)) / invHeight;

	//float x = glm::floor(width * uv.x);
	//float y = glm::floor(height * uv.y);

	////return bilinear(tx, ty, GetPixel(x, y), GetPixel(x + 1, y), GetPixel(x, y + 1), GetPixel(x + 1, y + 1));
	//return bilinear( ty, tx, GetPixel(x + 1, y + 1),GetPixel(x, y + 1), GetPixel(x + 1, y),  GetPixel(x, y));

	uv.x = uv.x * width + 0.5f;
	uv.y = uv.y * height + 0.5f;
	int x = floor(uv.x);
	int y = floor(uv.y);
	double u_ratio = uv.x - x;
	double v_ratio = uv.y - y;
	double u_opposite = 1 - u_ratio;
	double v_opposite = 1 - v_ratio;
	double result = (GetPixel(x, y).r * u_opposite + GetPixel(x + 1, y).r * u_ratio) * v_opposite +
		(GetPixel(x, y + 1).r * u_opposite + GetPixel(x + 1, y + 1).r * u_ratio) * v_ratio;

	return Color(result, result, result, result);
}

Color Texture2D::GetPixel(int x, int y)
{
	x = glm::clamp(x, 0, width - 1);
	y = glm::clamp(y, 0, height - 1);

	ilBindImage(ilTextureID);
	ILubyte* ilData = ilGetData();

	size_t startPos = (x + width * y) * bytePerPixel;

	BYTE* pixelData = new BYTE[bytePerPixel];
	memcpy_s(pixelData, bytePerPixel, ilData + startPos, bytePerPixel);

	Color pixelColor;
	if (format == Format::RGBA)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelColor.r = (float)pixelData[0] / 255.0f;
			pixelColor.g = (float)pixelData[1] / 255.0f;
			pixelColor.b = (float)pixelData[2] / 255.0f;
			pixelColor.a = (float)pixelData[3] / 255.0f;
		}
		if (dataType == DataType::Float)
		{
			memcpy_s(&pixelColor, 16, pixelData, 16);
		}
	}
	if (format == Format::BGRA)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelColor.b = (float)pixelData[0] / 255.0f;
			pixelColor.g = (float)pixelData[1] / 255.0f;
			pixelColor.r = (float)pixelData[2] / 255.0f;
			pixelColor.a = (float)pixelData[3] / 255.0f;
		}
		if (dataType == DataType::Float)
		{
			memcpy_s(&pixelColor, 16, pixelData, 16);
		}
	}
	if (format == Format::RGB)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelColor.r = (float)pixelData[0] / 255.0f;
			pixelColor.g = (float)pixelData[1] / 255.0f;
			pixelColor.b = (float)pixelData[2] / 255.0f;
			pixelColor.a = 1.0f;
		}
	}
	if (format == Format::BGR)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelColor.b = (float)pixelData[0] / 255.0f;
			pixelColor.g = (float)pixelData[1] / 255.0f;
			pixelColor.r = (float)pixelData[2] / 255.0f;
			pixelColor.a = 1.0f;
		}
	}
	if (format == Format::ColorIndex)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelColor.r = (float)pixelData[0] / 255.0f;
			pixelColor.g = (float)pixelData[0] / 255.0f;
			pixelColor.b = (float)pixelData[0] / 255.0f;
			pixelColor.a = (float)pixelData[0] / 255.0f;
		}
	}

	delete pixelData;

	return pixelColor;
}
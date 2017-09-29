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
}

Texture2D::~Texture2D()
{
	ilDeleteImage(ilTextureID);
	glDeleteTextures(1, &textureID);
	glDeleteSamplers(1, &samplerID);
}

Color Texture2D::GetPixel(int x, int y)
{
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
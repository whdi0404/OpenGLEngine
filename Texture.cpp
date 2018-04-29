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

	SetName(filename);
}

Texture2D::Texture2D(int width, int height, DataType dataType, Format format)
{
	ilTextureID = -1;
	// generate an image name
	ilGenImages(1, &ilTextureID);
	// bind it
	ilBindImage(ilTextureID);
	// match image origin to OpenGL’s
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	byte* data = new byte[width * height * GetBytePerPixel(dataType, format)];
	for(int i=0; i < width * height * GetBytePerPixel(dataType, format);++i)
	{
		data[i] = (byte)0xff;
	}
	ilTexImage(width, height, 0, FormatElemCountPerPixel(format), format, dataType, data);
	std::cout << ilGetInteger(IL_IMAGE_WIDTH) << std::endl;
	std::cout << ilGetInteger(IL_IMAGE_HEIGHT) << std::endl;
	std::cout << ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) << std::endl;
	std::cout << ilGetInteger(IL_IMAGE_FORMAT) << std::endl;
	std::cout << ilGetInteger(IL_IMAGE_TYPE) << std::endl;
	std::cout << ilGetInteger(IL_IMAGE_SIZE_OF_DATA) << std::endl;
	//ilSetPixels(0, 0, 0, width, height, 0, format, dataType, data);
	//ilBindImage(ilTextureID);

	this->width = width;
	this->height = width;
	bytePerPixel = GetBytePerPixel(dataType, format);
	this->format = format;
	this->dataType = dataType;

	/* Create and load textures to OpenGL */
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format,
		width,
		height,
		0, format, dataType,
		data);
	delete[] data;

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

	uv.x = uv.x * width - 0.5f;
	uv.y = uv.y * height - 0.5f;
	int x = floor(uv.x);
	int y = floor(uv.y);

	//return  GetPixel(x, y);

	double u_ratio = uv.x - x;
	double v_ratio = uv.y - y;
	double u_opposite = 1 - u_ratio;
	double v_opposite = 1 - v_ratio;
	double result = (GetPixel(x, y).r * u_opposite + GetPixel(x + 1, y).r * u_ratio) * v_opposite +
		(GetPixel(x, y + 1).r * u_opposite + GetPixel(x + 1, y + 1).r * u_ratio) * v_ratio;

	return Color(result, result, result, result);
}

void Texture2D::SetPixel(int x, int y, Color color)
{
	ilBindImage(ilTextureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	byte* pixelData = (byte*)malloc(bytePerPixel);

	size_t dataTypeSize = DataTypeBytePerPixel(dataType);

	Color pixelColor;
	if (format == Format::RGBA)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelData[dataTypeSize * 0] = (byte)(color.r * 255.0f);
			pixelData[dataTypeSize * 1] = (byte)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (byte)(color.b * 255.0f);
			pixelData[dataTypeSize * 3] = (byte)(color.a * 255.0f);
		}

		if (dataType == DataType::Float || dataType == DataType::Half || dataType == DataType::Double)
		{
			pixelData[dataTypeSize * 0] = color.r;
			pixelData[dataTypeSize * 1] = color.g;
			pixelData[dataTypeSize * 2] = color.b;
			pixelData[dataTypeSize * 3] = color.a;
		}

		if (dataType == DataType::Byte)
		{
			pixelData[dataTypeSize * 0] = (char)(color.r * 255.0f);
			pixelData[dataTypeSize * 1] = (char)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (char)(color.b * 255.0f);
			pixelData[dataTypeSize * 3] = (char)(color.a * 255.0f);
		}

		if (dataType == DataType::Int || dataType == DataType::Byte || dataType == DataType::Short)
		{
			pixelData[dataTypeSize * 0] = (int)color.r;
			pixelData[dataTypeSize * 1] = (int)color.g;
			pixelData[dataTypeSize * 2] = (int)color.b;
			pixelData[dataTypeSize * 3] = (int)color.a;
		}
	}
	if (format == Format::BGRA)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelData[dataTypeSize * 0] = (byte)(color.b * 255.0f);
			pixelData[dataTypeSize * 1] = (byte)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (byte)(color.r * 255.0f);
			pixelData[dataTypeSize * 3] = (byte)(color.a * 255.0f);
		}

		if (dataType == DataType::Float || dataType == DataType::Half || dataType == DataType::Double)
		{
			pixelData[dataTypeSize * 0] = color.b;
			pixelData[dataTypeSize * 1] = color.g;
			pixelData[dataTypeSize * 2] = color.r;
			pixelData[dataTypeSize * 3] = color.a;
		}

		if (dataType == DataType::Byte)
		{
			pixelData[dataTypeSize * 0] = (char)(color.b * 255.0f);
			pixelData[dataTypeSize * 1] = (char)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (char)(color.r * 255.0f);
			pixelData[dataTypeSize * 3] = (char)(color.a * 255.0f);
		}

		if (dataType == DataType::Int || dataType == DataType::Byte || dataType == DataType::Short)
		{
			pixelData[dataTypeSize * 0] = (int)color.b;
			pixelData[dataTypeSize * 1] = (int)color.g;
			pixelData[dataTypeSize * 2] = (int)color.r;
			pixelData[dataTypeSize * 3] = (int)color.a;
		}
	}
	if (format == Format::RGB)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelData[dataTypeSize * 0] = (byte)(color.r * 255.0f);
			pixelData[dataTypeSize * 1] = (byte)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (byte)(color.b * 255.0f);
		}

		if (dataType == DataType::Float || dataType == DataType::Half || dataType == DataType::Double)
		{
			pixelData[dataTypeSize * 0] = color.r;
			pixelData[dataTypeSize * 1] = color.g;
			pixelData[dataTypeSize * 2] = color.b;
		}

		if (dataType == DataType::Byte)
		{
			pixelData[dataTypeSize * 0] = (char)(color.r * 255.0f);
			pixelData[dataTypeSize * 1] = (char)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (char)(color.b * 255.0f);
		}

		if (dataType == DataType::Int || dataType == DataType::Byte || dataType == DataType::Short)
		{
			pixelData[dataTypeSize * 0] = (int)color.r;
			pixelData[dataTypeSize * 1] = (int)color.g;
			pixelData[dataTypeSize * 2] = (int)color.b;
		}
	}
	if (format == Format::BGR)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelData[dataTypeSize * 0] = (byte)(color.b * 255.0f);
			pixelData[dataTypeSize * 1] = (byte)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (byte)(color.r * 255.0f);
		}

		if (dataType == DataType::Float || dataType == DataType::Half || dataType == DataType::Double)
		{
			pixelData[dataTypeSize * 0] = color.b;
			pixelData[dataTypeSize * 1] = color.g;
			pixelData[dataTypeSize * 2] = color.r;
		}

		if (dataType == DataType::Byte)
		{
			pixelData[dataTypeSize * 0] = (char)(color.b * 255.0f);
			pixelData[dataTypeSize * 1] = (char)(color.g * 255.0f);
			pixelData[dataTypeSize * 2] = (char)(color.r * 255.0f);
		}

		if (dataType == DataType::Int || dataType == DataType::Byte || dataType == DataType::Short)
		{
			pixelData[dataTypeSize * 0] = (int)color.b;
			pixelData[dataTypeSize * 1] = (int)color.g;
			pixelData[dataTypeSize * 2] = (int)color.r;
		}
	}
	if (format == Format::ColorIndex || format == Format::Alpha)
	{
		if (dataType == DataType::Unsigned_Byte)
		{
			pixelData[dataTypeSize * 0] = (byte)(color.r * 255.0f);
		}

		if (dataType == DataType::Float || dataType == DataType::Half || dataType == DataType::Double)
		{
			pixelData[dataTypeSize * 0] = color.r;
		}

		if (dataType == DataType::Byte)
		{
			pixelData[dataTypeSize * 0] = (char)(color.r * 255.0f);
		}

		if (dataType == DataType::Int)
		{
			pixelData[dataTypeSize * 0] = (int)color.r;
		}

		if (dataType == DataType::Short)
		{
			pixelData[dataTypeSize * 0] = (short)color.r;
		}
	}

	ilSetPixels(x, y, 0, 1, 1, 0, format, dataType, pixelData);
	
	free(pixelData);
}

void Texture2D::ApplyChange()
{
	ilBindImage(ilTextureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format,
		width,
		height,
		0, format, dataType,
		ilGetData());
}

Color Texture2D::GetPixel(int x, int y)
{
	x = glm::clamp(x, 0, width - 1);
	y = glm::clamp(y, 0, height - 1);

	ilBindImage(ilTextureID);
	ILubyte* ilData = ilGetData();

	size_t startPos = (y * width + x) * bytePerPixel;

	BYTE* pixelData = ilData + startPos;

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

	return pixelColor;
}

Texture2D * Texture2D::CreateNormalTexture(Texture2D* heightmap, float tileScale, float heightScale)
{
	int w = heightmap->width;
	int h = heightmap->height;

	Texture2D* normalTexture = new Texture2D(w, h, DataType::Unsigned_Byte, Format::RGBA);

	static float max = FLT_MIN;
	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			float height = heightmap->GetPixelBilinear(glm::vec2((float)x / w, (float)y / h)).r * heightScale;

			float right = height;
			float down = height;
			if (x + 1 < w)
			{
				right = heightmap->GetPixelBilinear(glm::vec2((float)(x + 1) / w, (float)y / h)).r * heightScale;
			}
			/*if (x - 1 >= 0)
			{
			left = heightmap->GetPixel(x - 1, y).r * heightScale;
			}
			if (y + 1 < h)
			{
			up = heightmap->GetPixel(x, y + 1).r * heightScale;
			}*/
			if (y - 1 >= 0)
			{
				down = heightmap->GetPixelBilinear(glm::vec2((float)x / w, (float)(y - 1) / h)).r * heightScale;
			}
			glm::vec3 normal = -glm::normalize(glm::cross(glm::vec3(0, height, 0) - glm::vec3(0, down, -tileScale), glm::vec3(0, height, 0) - glm::vec3(tileScale, right, 0)));
			normal = (normal + 1.0f) * 0.5f;
			Color c = Color(normal.x, normal.y, normal.z);
			normalTexture->SetPixel(x, y, c);
			max = glm::max(max, (float)x);
		}
	}
	normalTexture->ApplyChange();
	std::cout << "max: " << max << std::endl;

	return normalTexture;
}

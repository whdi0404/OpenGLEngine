#include "stdafx.h"
#include "Texture.h"
#include "FBM.h"
#include "MemoryUtil.h"

using namespace FractalUtil;

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
	for (int i = 0; i < width * height * GetBytePerPixel(dataType, format); ++i)
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
	bytePerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
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

	byte f2B[4]{ 0,0,0,0 };

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

		if (dataType == DataType::Float)
		{
			MemoryUtil::float2Bytes(color.r, f2B);
			pixelData[0] = f2B[0];
			pixelData[1] = f2B[1];
			pixelData[2] = f2B[2];
			pixelData[3] = f2B[3];

			MemoryUtil::float2Bytes(color.g, f2B);
			pixelData[4] = f2B[0];
			pixelData[5] = f2B[1];
			pixelData[6] = f2B[2];
			pixelData[7] = f2B[3];

			MemoryUtil::float2Bytes(color.b, f2B);
			pixelData[8] = f2B[0];
			pixelData[9] = f2B[1];
			pixelData[10] = f2B[2];
			pixelData[11] = f2B[3];

			MemoryUtil::float2Bytes(color.a, f2B);
			pixelData[12] = f2B[0];
			pixelData[13] = f2B[1];
			pixelData[14] = f2B[2];
			pixelData[15] = f2B[3];
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

		if (dataType == DataType::Float)
		{
			MemoryUtil::float2Bytes(color.b, f2B);
			pixelData[0] = f2B[0];
			pixelData[1] = f2B[1];
			pixelData[2] = f2B[2];
			pixelData[3] = f2B[3];

			MemoryUtil::float2Bytes(color.g, f2B);
			pixelData[4] = f2B[0];
			pixelData[5] = f2B[1];
			pixelData[6] = f2B[2];
			pixelData[7] = f2B[3];

			MemoryUtil::float2Bytes(color.r, f2B);
			pixelData[8] = f2B[0];
			pixelData[9] = f2B[1];
			pixelData[10] = f2B[2];
			pixelData[11] = f2B[3];

			MemoryUtil::float2Bytes(color.a, f2B);
			pixelData[12] = f2B[0];
			pixelData[13] = f2B[1];
			pixelData[14] = f2B[2];
			pixelData[15] = f2B[3];
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

		if (dataType == DataType::Float)
		{
			MemoryUtil::float2Bytes(color.r, f2B);
			pixelData[0] = f2B[0];
			pixelData[1] = f2B[1];
			pixelData[2] = f2B[2];
			pixelData[3] = f2B[3];

			MemoryUtil::float2Bytes(color.g, f2B);
			pixelData[4] = f2B[0];
			pixelData[5] = f2B[1];
			pixelData[6] = f2B[2];
			pixelData[7] = f2B[3];

			MemoryUtil::float2Bytes(color.b, f2B);
			pixelData[8] = f2B[0];
			pixelData[9] = f2B[1];
			pixelData[10] = f2B[2];
			pixelData[11] = f2B[3];
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

		if (dataType == DataType::Float)
		{
			pixelData[dataTypeSize * 0] = color.b;
			pixelData[dataTypeSize * 1] = color.g;
			pixelData[dataTypeSize * 2] = color.r;


			MemoryUtil::float2Bytes(color.b, f2B);
			pixelData[0] = f2B[0];
			pixelData[1] = f2B[1];
			pixelData[2] = f2B[2];
			pixelData[3] = f2B[3];

			MemoryUtil::float2Bytes(color.g, f2B);
			pixelData[4] = f2B[0];
			pixelData[5] = f2B[1];
			pixelData[6] = f2B[2];
			pixelData[7] = f2B[3];

			MemoryUtil::float2Bytes(color.r, f2B);
			pixelData[8] = f2B[0];
			pixelData[9] = f2B[1];
			pixelData[10] = f2B[2];
			pixelData[11] = f2B[3];
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

		if (dataType == DataType::Float)
		{
			pixelData[dataTypeSize * 0] = color.r;

			MemoryUtil::float2Bytes(color.r, f2B);
			pixelData[0] = f2B[0];
			pixelData[1] = f2B[1];
			pixelData[2] = f2B[2];
			pixelData[3] = f2B[3];
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
//heightSeed: 개략적인 높이를 구성함, surfaceSeed: 개략적인 표면의 종류(풀, 흙 등)를 결정
Texture2D * Texture2D::CreateRandomHeightmap(int width, int height, int detail, float H, Texture2D* heightSeed, Texture2D* surfaceSeed, bool isFractal)
{
	Texture2D* texture = new Texture2D(width, height, DataType::Float, Format::RGBA);

	float* pixels = new float[width * height];

	float sam;

	Texture2D* smoothHeightSeed = TextureUtil::GaussianSmoothing(heightSeed, 20, 0.1f);

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			float uvX = (float)x / width;
			float uvY = (float)y / height;
			float h = smoothHeightSeed->GetPixelBilinear(glm::vec2(uvX, uvY)).r;
			pixels[y * width + x] = h * H * detail * 0.2f;
		}
	}

	float min = FLT_MAX;
	float max = FLT_MIN;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			min = glm::min(min, pixels[y * width + x]);
			max = glm::max(max, pixels[y * width + x]);
		}
	}
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;


	int maxPow = pow(2, detail * 2);
	if (detail > 0)
	{
		for (int d = detail; d > glm::max(0, detail - 3); --d)
		{
			int lerp = d;

			int pow2d = pow(2, lerp);

			int randSeed = rand() % 10000;
			int w = width / pow2d;
			int h = height / pow2d;
			//d: 낮을수록 디테일해짐

			Texture2D* tex = new Texture2D(w, h, DataType::Float, Format::RGBA);
			float smallH = H * d / detail * 0.4f;

			for (int x = 0; x < w; ++x)
			{
				for (int y = 0; y < h; ++y)
				{
					float uvX = (float)x / width;
					float uvY = (float)y / height;
					float weight = smoothHeightSeed->GetPixelBilinear(glm::vec2(uvX, uvY)).g;
					float c;
					if (isFractal)
					{
						c = Multi_fractal(randSeed + (float)x / w, randSeed + (float)y / h, 3, smallH * weight, (w + h) / 2, 1.0f, 1.0f);
					}
					else
					{
						c = FBM(randSeed + (float)x / w, randSeed + (float)y / h, 3, smallH * weight, (w + h) / 2, 1.0f);
					}
					tex->SetPixel(x, y, Color(c, c, c));
					Color color = tex->GetPixel(x - 1, y - 1);
					color.r = 0;
				}
			}
			tex->ApplyChange();

			for (int x = 0; x < width; ++x)
			{
				for (int y = 0; y < height; ++y)
				{
					float uvX = (float)x / width;
					float uvY = (float)y / height;
					float h = tex->GetPixelBilinear(glm::vec2(uvX, uvY)).r;

					pixels[y * width + x] += h;
				}
			}
			delete tex;
		}
	}

	delete smoothHeightSeed;


	min = FLT_MAX;
	max = FLT_MIN;
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			min = glm::min(min, pixels[y * width + x]);
			max = glm::max(max, pixels[y * width + x]);
		}
	}

	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			float v = pixels[y * width + x];
			texture->SetPixel(x, y, Color(v, v, v));
		}
	}

	min = FLT_MAX;
	max = FLT_MIN;
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			min = glm::min(min, texture->GetPixel(x, y).r);
			max = glm::max(max, texture->GetPixel(x, y).r);
		}
	}
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;

	if (isFractal)
	{
		Texture2D* tex = texture;
		texture = TextureUtil::GaussianSmoothing(texture, 5, 1.0f); 
		delete tex;
	}

	min = FLT_MAX;
	max = FLT_MIN;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			min = glm::min(min, texture->GetPixel(x, y).r);
			max = glm::max(max, texture->GetPixel(x, y).r);
		}
	}
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			float p = texture->GetPixel(x, y).r;
			float v = inverseLerp(min, max, p);
			texture->SetPixel(x, y, Color(v, v, v));
		}
	}

	texture->ApplyChange();

	return texture;
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

Texture2D * Texture2D::CreateNormalTexture(Texture2D* heightmap, int width, int height, float tileScale, float heightScale)
{
	double w = width;
	double h = height;

	double scaling = w / heightmap->GetWidth();

	glm::dvec2 pixelHalf = glm::dvec2(1.0 / w * 0.5f, 1.0 / h * 0.5f);

	Texture2D* normalTexture = new Texture2D(w, h, DataType::Float, Format::RGBA);

	static double max = FLT_MIN;
	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			//겹치는 버텍스들 보간해야함.
			//ex) 2,3정점의 노멀은 1,2,4,5사각형의 노멀과 2,3,5,6노멀에 똑같이 영향받아야함.
			//1-----------2-----------3
			///           /			  /
			/// 		  /			  /
			//4-----------5-----------6
			double height = heightmap->GetPixelBilinear(glm::dvec2((double)x / w, (double)y / h) + pixelHalf).r * heightScale;

			double right = height;
			double down = height;
			double up = height;
			double left = height;
			if (x + 1 < w)
			{
				right = heightmap->GetPixelBilinear(glm::dvec2((double)(x + 1) / w, (double)y / h) + pixelHalf).r * heightScale;
			}
			/*if (x - 1 >= 0)
			{
			left = heightmap->GetPixel(x - 1, y).r * heightScale;
			}
			*/
			if (y + 1 < h)
			{
				up = heightmap->GetPixelBilinear(glm::dvec2((double)x / w, (double)(y + 1) / h) + pixelHalf).r * heightScale;
			}
			/*if (y - 1 >= 0)
			{
				down = heightmap->GetPixelBilinear(glm::dvec2((double)x / w, (double)(y - 1) / h)).r * heightScale;
			}*/
			glm::dvec3 normal = glm::normalize(glm::cross(
				glm::dvec3(0, height, 0) - glm::dvec3(0, up, tileScale / scaling)
				, glm::dvec3(0, height, 0) - glm::dvec3(tileScale / scaling, right, 0)));
			normalTexture->SetPixel(x, y, Color(normal.r, normal.g, normal.b));
		}
	}
	normalTexture->ApplyChange();

	return normalTexture;
}

Texture2D * Texture2D::ResizeTexture(Texture2D * heightmap, int width, int height)
{
	int w = width;
	int h = height;

	Texture2D* resizeTexture = new Texture2D(w, h, DataType::Float, Format::RGBA);

	static float max = FLT_MIN;
	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			Color c = heightmap->GetPixelBilinear(glm::vec2((float)x / w, (float)y / h));
			resizeTexture->SetPixel(x, y, c);
		}
	}
	resizeTexture->ApplyChange();

	return resizeTexture;
}

#include "stdafx.h"
#include "TextureUtil.h"
#include "Texture.h"


Colord FilterCreation(Colord colors[][5], double sigma)
{
	Colord retVal = Colord();
	// intialising standard deviation to 1.0
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization

	double** GKernel;
	static std::map<double, double**> values;
	if (values.find(sigma) == values.end())
	{
		GKernel = new double*[5];
		for (int i = 0; i < 5; i++)
		{
			GKernel[i] = new double[5];
		}

		double sum = 0;
		for (int x = -2; x <= 2; x++)
		{
			for (int y = -2; y <= 2; y++)
			{
				r = sqrt(x*x + y * y);
				GKernel[x + 2][y + 2] = (exp(-(r*r) / s)) / (PxPi * s);

				sum += GKernel[x + 2][y + 2];
			}
		}

		for (int i = 0; i < 5; ++i)
			for (int j = 0; j < 5; ++j)
				GKernel[i][j] /= sum;

		values.insert(std::make_pair(sigma, GKernel));
	}
	else
	{
		GKernel = values[sigma];
	}

	// normalising the Kernel
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			retVal.r += colors[i][j].r * GKernel[i][j];
			retVal.g += colors[i][j].g * GKernel[i][j];
			retVal.b += colors[i][j].b * GKernel[i][j];
			retVal.a += colors[i][j].a * GKernel[i][j];
		}

	return retVal;
}

Texture2D* TextureUtil::GaussianSmoothing(Texture2D* texture, int smth, double sigma)
{
	Texture2D* tempTex1 = CopyTexture(texture);
	Texture2D* tempTex2 = new Texture2D(texture->GetWidth(), texture->GetHeight(), texture->GetDataType(), texture->GetFormat());

	Texture2D* targetTexture = nullptr;
	Texture2D* retTexture = nullptr;
	bool swch = false;

	for (int smooth = 0; smooth < smth; ++smooth)
	{
		targetTexture = swch ? tempTex2 : tempTex1;
		retTexture = swch ? tempTex1 : tempTex2;

		for (int x = 0; x < texture->GetWidth(); ++x)
		{
			for (int y = 0; y < texture->GetHeight(); ++y)
			{
				Colord colors[5][5];
				for (int i = -2; i <= 2; ++i)
					for (int j = -2; j <= 2; ++j)
					{
						Color tempColor = targetTexture->GetPixel(x + i, y + j);
						colors[i + 2][j + 2] = Colord(tempColor.r, tempColor.g, tempColor.b, tempColor.a);
					}
				Colord smoothColor = FilterCreation(colors, sigma);
				retTexture->SetPixel(x, y, Color(smoothColor.r, smoothColor.g, smoothColor.b, smoothColor.a));
			}
		}
		swch = !swch;
	}

	if(targetTexture != nullptr)
		delete targetTexture;

	return retTexture;
}

Texture2D * TextureUtil::CopyTexture(Texture2D * texture)
{
	Texture2D* retTex = new Texture2D(texture->GetWidth(), texture->GetHeight(), texture->GetDataType(), texture->GetFormat());
	for (int x = 0; x < texture->GetWidth(); ++x)
	{
		for (int y = 0; y < texture->GetHeight(); ++y)
		{
			retTex->SetPixel(x, y, texture->GetPixel(x, y));
		}
	}
	return retTex;
}

#include "stdafx.h"
#include "FBM.h"

float FractalUtil::random(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13 * n);
	float ret;
	ret = (1 - ((n * (n * n * 19417 / 189851) + 4967243) & 4945007) / 3354521.0);
	return ret;
}

double FractalUtil::cosineinterpolation(double x, double y, double t)
{
	double ft;
	double f;
	double ret;
	ft = t * physx::PxPi;
	f = (1 - cos(ft)) * 0.5f;
	ret = x * 1 - f + y * f;
	return ret;
}

float FractalUtil::noise(float x, float y)
{
	int xinteger = x;
	float fractionx = x - xinteger;
	int yinteger = y;
	float fractiony = y - yinteger;
	float v1, v2, v3, v4, i1, i2;
	float ret;

	v1 = smoothrandom(xinteger, yinteger);//smoothrandom
	v2 = smoothrandom(xinteger + 1, yinteger);
	v3 = smoothrandom(xinteger, yinteger + 1);
	v4 = smoothrandom(xinteger + 1, yinteger + 1);
	i1 = cosineinterpolation(v1, v2, fractionx);
	i2 = cosineinterpolation(v3, v4, fractionx);
	ret = cosineinterpolation(i1, i2, fractiony);

	return ret;
}

float FractalUtil::smoothrandom(int x, int y)
{
	float corners = (random(x - 1, y - 1) + random(x + 1, y - 1) + random(x - 1, y + 1) + random(x + 1, y + 1)) / 16;
	float sides = (random(x - 1, y) + random(x + 1, y) + random(x, y - 1) + random(x, y + 1)) / 8;

	float center = random(x, y) / 4;
	float ret = corners + sides + center;

	return ret;
}

float FractalUtil::FBM(float x, float y, float octaves, float amplitude, float frequency, float h)
{
	float ret = 0;

	for (int i = 0; i < (octaves - 1); ++i)
	{
		ret += (noise(x * frequency, y*frequency) * amplitude);
		amplitude *= h;
	}
	return ret;
}

float FractalUtil::Multi_fractal(float x, float y, float octaves, float amplitude, float frequency, float h, float offset)
{
	float ret = 1;

	for (int i = 0; i < (octaves - 1); ++i)
	{
		ret *= (offset)* (noise(x * frequency, y * frequency) * amplitude);
		amplitude *= h;
	}
	return ret;
}

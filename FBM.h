#pragma once

namespace FractalUtil
{
	float random(int x, int y);

	double cosineinterpolation(double number1, double number2, double x);
	
	float noise(float x, float y);

	float smoothrandom(int x, int y);

	float FBM(float x, float y, float octaves, float amplitude, float frequency, float h);

	float Multi_fractal(float x, float y, float octaves, float amplitude, float frequency, float h, float offset);
}

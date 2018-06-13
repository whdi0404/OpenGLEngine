#pragma once

class Texture2D;
namespace TextureUtil
{
	template<typename T>
	class tColor
	{
	public:
		tColor() : r(0), g(0), b(0), a(0)
		{
		}
		tColor(T r, T g, T b, T a = 1.0f)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
	public:
		T r, g, b, a;
	};

	template<>
	class tColor<double>
	{
	public:
		tColor() : r(0), g(0), b(0), a(0)
		{
		}
		tColor(double r, double g, double b, double a = 1.0f)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
	public:
		double r, g, b, a;

		tColor<double> bilinear(
			const double &tx,
			const double &ty,
			const tColor<double> &c00,
			const tColor<double> &c10,
			const tColor<double> &c01,
			const tColor<double> &c11)
		{
			glm::dvec4 colVec =
				(1.0f - tx) * (1.0f - ty) * glm::dvec4(c00.r, c00.g, c00.b, c00.a) +
				tx * (1.0f - ty) * glm::dvec4(c10.r, c10.g, c10.b, c10.a) +
				(1.0f - tx) * ty * glm::dvec4(c01.r, c01.g, c01.b, c01.a) +
				tx * ty * glm::dvec4(c11.r, c11.g, c11.b, c11.a);

			return tColor<double>(glm::floor(colVec.r * 255) / 255.0f, glm::floor(colVec.g * 255) / 255.0f, glm::floor(colVec.b * 255) / 255.0f, glm::floor(colVec.a * 255) / 255.0f);
		}
	};


	template<>
	class tColor<float>
	{
	public:
		tColor() : r(0), g(0), b(0), a(0)
		{
		}
		tColor(float r, float g, float b, float a = 1.0f)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
	public:
		float r, g, b, a;

		tColor<float> bilinear(
			const float &tx,
			const float &ty,
			const tColor<float> &c00,
			const tColor<float> &c10,
			const tColor<float> &c01,
			const tColor<float> &c11)
		{
			glm::vec4 colVec =
				(1.0f - tx) * (1.0f - ty) * glm::vec4(c00.r, c00.g, c00.b, c00.a) +
				tx * (1.0f - ty) * glm::vec4(c10.r, c10.g, c10.b, c10.a) +
				(1.0f - tx) * ty * glm::vec4(c01.r, c01.g, c01.b, c01.a) +
				tx * ty * glm::vec4(c11.r, c11.g, c11.b, c11.a);

			return tColor<float>(glm::floor(colVec.r * 255) / 255.0f, glm::floor(colVec.g * 255) / 255.0f, glm::floor(colVec.b * 255) / 255.0f, glm::floor(colVec.a * 255) / 255.0f);
		}
	};

	using Color = tColor<float>;
	using Colord = tColor<double>;

	Texture2D* GaussianSmoothing(Texture2D* texture, int smooth, double sigma);

	Texture2D* CopyTexture(Texture2D* texture);
}
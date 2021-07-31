/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERGBColor.h: struct that represents a RGB color
/*=============================================================================*/
#ifndef ELITE_MATH_RGBCOLOR
#define	ELITE_MATH_RGBCOLOR

//Project includes
#include "EMathUtilities.h"
#include <algorithm>
#include <cmath>

namespace Elite
{
	struct RGBColor final
	{
		//=== Datamembers ===
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;

		//=== Constructors & Destructor ===
		RGBColor() = default;
		RGBColor(float _r, float _g, float _b) :r(_r), g(_g), b(_b) {}
		RGBColor(const RGBColor& c) : r(c.r), g(c.g), b(c.b) {}
		RGBColor(RGBColor&& c) noexcept : r(std::move(c.r)), g(std::move(c.g)), b(std::move(c.b)) {}
		~RGBColor() = default;

		//=== Operators ===
		RGBColor& operator=(const RGBColor& c)
		{ r = c.r; g = c.g; b = c.b; return *this; }
		RGBColor& operator=(RGBColor&& c) noexcept
		{ r = std::move(c.r); g = std::move(c.g); b = std::move(c.b); return *this;	}

		//=== Arithmetic Operators ===
		inline RGBColor operator+(const RGBColor& c) const
		{ return RGBColor(r + c.r, g + c.g, b + c.b); }
		inline RGBColor operator-(const RGBColor& c) const 
		{ return RGBColor(r - c.r, g - c.g, b - c.b); }
		inline RGBColor operator*(const RGBColor& c) const 
		{ return RGBColor(r * c.r, g * c.g, b * c.b); }
		inline RGBColor operator/(float f) const
		{
			float rev = 1.0f / f;
			return RGBColor(r * rev, g * rev, b * rev);
		}
		inline RGBColor operator*(float f) const
		{ return RGBColor(r * f, g * f, b * f);	}
		inline RGBColor operator/(const RGBColor& c) const
		{ return RGBColor(r / c.r, g / c.g, b / c.b); }

		//=== Compound Assignment Operators ===
		inline RGBColor& operator+=(const RGBColor& c)
		{ r += c.r; g += c.g; b += c.b; return *this; }
		inline RGBColor& operator-=(const RGBColor& c)
		{ r -= c.r; g -= c.g; b -= c.b; return *this; }
		inline RGBColor& operator*=(const RGBColor& c)
		{ r *= c.r; g *= c.g; b *= c.b; return *this; }
		inline RGBColor& operator/=(const RGBColor& c)
		{ r /= c.r; g /= c.g; b /= c.b; return *this; }
		inline RGBColor& operator*=(float f)
		{ r *= f; g *= f; b *= f; return *this; }
		inline RGBColor& operator/=(float f)
		{
			float rev = 1.0f / f;
			r *= rev; g *= rev; b *= rev; return *this;
		}

		//=== Relational Operators ===
		inline bool operator==(const RGBColor& c) const
		{ return AreEqual(r, c.r) && AreEqual(g, c.g) && AreEqual(b, c.b); }
		inline bool operator!=(const RGBColor& c) const
		{ return !(*this == c);	}

		//=== Internal RGBColor Functions ===
		inline void Clamp()
		{
			r = Elite::Clamp(r, 0.0f, 1.0f);
			g = Elite::Clamp(g, 0.0f, 1.0f);
			b = Elite::Clamp(b, 0.0f, 1.0f);
		}

		inline void MaxToOne()
		{
			float maxValue = std::max(r, std::max(g, b));
			if (maxValue > 1.f)
				*this /= maxValue;
		}
	};

	//=== Global RGBColor Functions ===
	inline RGBColor Max(const RGBColor& c1, const RGBColor& c2)
	{
		RGBColor c = c1;
		if (c2.r > c.r) c.r = c2.r;
		if (c2.g > c.g) c.g = c2.g;
		if (c2.b > c.b) c.b = c2.b;
		return c;
	}

	inline RGBColor Min(const RGBColor& c1, const RGBColor& c2)
	{
		RGBColor c = c1;
		if (c2.r < c.r) c.r = c2.r;
		if (c2.g < c.g) c.g = c2.g;
		if (c2.b < c.b) c.b = c2.b;
		return c;
	}

	inline uint32_t GetSDL_ARGBColor(const RGBColor& c)
	{
		RGBColor rsColor = c * 255;
		uint32_t finalColor = 0;
		finalColor |= (uint8_t)rsColor.b;
		finalColor |= (uint8_t)rsColor.g << 8;
		finalColor |= (uint8_t)rsColor.r << 16;
		return finalColor;
	}

	inline RGBColor GetColorFromSDL_ARGB(const uint32_t c)
	{
		RGBColor color =
		{
			float(uint8_t(c >> 16)) / 255.f,
			float(uint8_t(c >> 8)) / 255.f,
			float(uint8_t(c)) / 255.f
		};
		return color;
	}

	inline RGBColor GammaCorrection(const RGBColor& c)
	{
		RGBColor result = c;
		float gamma = 1 / 2.2f;
		result.r = std::powf(result.r, gamma);
		result.g = std::powf(result.g, gamma);
		result.b = std::powf(result.b, gamma);
		result.MaxToOne();
		return result;
	}

	inline RGBColor GammaCorrectionSRGB(const RGBColor& c)
	{
		RGBColor result = c;
		if (result.r <= 0.0031308f)
			result.r *= 12.92f;
		else
			result.r = 1.055f * powf(result.r, 1 / 2.4f) - 0.055f;
		if (result.g <= 0.0031308f)
			result.g *= 12.92f;
		else
			result.g = 1.055f * powf(result.g, 1 / 2.4f) - 0.055f;
		if (result.b <= 0.0031308f)
			result.b *= 12.92f;
		else
			result.b = 1.055f * powf(result.b, 1 / 2.4f) - 0.055f;
		result.MaxToOne();
		return result;
	}
}
#endif
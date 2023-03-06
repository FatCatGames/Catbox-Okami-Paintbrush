#pragma once

#include "../External/rapidjson/rapidjson.h"
#include "../External/rapidjson/document.h"

namespace Catbox
{
	template<typename T>
	class Vector4;
}

class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	Color();
	Color(const float& aRed, const float& aGreen, const float& aBlue, const float& anAlpha);
	void operator=(const Color& aColor);
	void operator=(const float aColor[4]);

	inline static Color Red() { return Color{ 1,0,0,1 }; }
	inline static Color Orange() { return Color{ 1,0.7f,0,1 }; }
	inline static Color Yellow() { return Color{ 1,1,0,1 }; }
	inline static Color Green() { return Color{ 0,1,0,1 }; }
	inline static Color Blue() { return Color{ 0,0,1,1 }; }
	inline static Color Cyan() { return Color{ 0,1,1,1 }; }
	inline static Color Black() { return Color{ 0,0,0,1 }; }
	inline static Color White() { return Color{ 1,1,1,1 }; }
	inline static Color Magenta() { return Color{ 1,0,1,1 }; }
	inline static Color Purple() { return Color{ 0.5f,0,0.5f,1 }; }
	inline static Color Pink() { return Color{ 0.95f,0.6f,0.7f,1 }; }

	void LoadFromJson(rapidjson::Value& aJsonObject);
	void ParseToJsonObject(rapidjson::Value& aJsonObject, rapidjson::Document& aOutput, const char* aName);
};

inline Color operator*(const float& aValue, const Color& aColor) 
{
	return { aColor.r * aValue, aColor.g * aValue, aColor.b * aValue, aColor.a * aValue };
}

inline Color operator*(const Color& aColor, const float& aValue)
{
	return { aColor.r * aValue, aColor.g * aValue, aColor.b * aValue, aColor.a * aValue };
}

inline Color operator/(const float& aValue, const Color& aColor)
{
	return { aColor.r / aValue, aColor.g / aValue, aColor.b / aValue, aColor.a / aValue };
}

inline Color operator/(const Color& aColor, const float& aValue)
{
	return { aColor.r / aValue, aColor.g / aValue, aColor.b / aValue, aColor.a / aValue };
}

inline Color operator-(const Color& aColor1, const Color& aColor2)
{
	return { aColor1.r - aColor2.r, aColor1.g - aColor2.g, aColor1.b - aColor2.b, aColor1.a - aColor2.a };
}

inline Color operator+(const Color& aColor1, const Color& aColor2)
{
	return { aColor1.r + aColor2.r, aColor1.g + aColor2.g, aColor1.b + aColor2.b, aColor1.a + aColor2.a };
}

inline Color operator*(const Color& aColor1, const Color& aColor2)
{
	return { aColor1.r * aColor2.r, aColor1.g * aColor2.g, aColor1.b * aColor2.b, aColor1.a * aColor2.a };
}

inline Color operator/(const Color& aColor1, const Color& aColor2)
{
	return { aColor1.r / aColor2.r, aColor1.g / aColor2.g, aColor1.b / aColor2.b, aColor1.a / aColor2.a };
}

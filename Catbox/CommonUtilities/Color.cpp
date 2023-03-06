#include "stdafx.h"
#include "Color.h"
#include "Vector4.hpp"


Color::Color()
{
	r = g = b = a = 1;
}

Color::Color(const float& aRed, const float& aGreen, const float& aBlue, const float& anAlpha)
{
	r = aRed;
	g = aGreen;
	b = aBlue;
	a = anAlpha;
}

void Color::operator=(const Color& aColor)
{
	r = aColor.r;
	g = aColor.g;
	b = aColor.b;
	a = aColor.a;
}

void Color::operator=(const float aColor[4])
{
	r = aColor[0];
	g = aColor[1];
	b = aColor[2];
	a = aColor[3];
}

void Color::LoadFromJson(rapidjson::Value& aJsonObject)
{
	r = aJsonObject["r"].GetFloat();
	g = aJsonObject["g"].GetFloat();
	b = aJsonObject["b"].GetFloat();
	a = aJsonObject["a"].GetFloat();
}

void Color::ParseToJsonObject(rapidjson::Value& aJsonObject, rapidjson::Document& aOutput, const char* aName)
{
	rapidjson::GenericStringRef<char> name = rapidjson::GenericStringRef<char>(aName);
	rapidjson::Value rgba;
	auto& alloc = aOutput.GetAllocator();
	rgba.SetObject();
	rgba.AddMember("r", r, alloc);
	rgba.AddMember("g", g, alloc);
	rgba.AddMember("b", b, alloc);
	rgba.AddMember("a", a, alloc);

	aJsonObject.AddMember(name, rgba, alloc);
}

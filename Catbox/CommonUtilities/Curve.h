#pragma once
#include <vector>
#include "../External/rapidjson/rapidjson.h"
#include "../External/rapidjson/document.h"

class Curve
{
public:
	struct Coord 
	{
		float x;
		float y;
	};

	enum InterpolationMode
	{
		Linear,
		Cubic
	};

	Curve(const std::vector<Coord>& someCoords);
	Curve() = default;
	~Curve() = default;

	float Evaluate(float aX);
	void AddCoord(const Coord& aCoord);
	void RenderInProperties(const string& aLabel);
	void LoadFromJson(rapidjson::Value& aJsonObject);
	void ParseToJsonObject(rapidjson::Value& aJsonObject, rapidjson::Document& aOutput, const char* aName);
	void SetMin(int aMin);
	void SetMax(int aMax);

private:
	InterpolationMode myInterpolationMode = InterpolationMode::Linear;
	std::vector<Coord> myCoords;
	bool myEditActive = false;
	int myMin = -100;
	int myMax = 100;
};
#pragma once
#include <vector>
#include "../External/rapidjson/rapidjson.h"
#include "../External/rapidjson/document.h"
#include "CommonUtilities/Color.h"

struct ColorPoint
{
	Color color;
	float pos; //0 to 1
};

class ColorGradient
{

public:
	ColorGradient();
	~ColorGradient() = default;
	void AddColor(const Color& aColor, float aPosition);
	const Color Evaluate(float aPosition);
	inline std::vector<ColorPoint>& GetColors() { return myColors; }
	void RemoveColor(int anIndex);
	void RemoveColor(ColorPoint& aColor);
	void Sort();
	void LoadFromJson(rapidjson::Value& aJsonObject);
	void ParseToJsonObject(rapidjson::Value& aEmissionData, rapidjson::Document& aOutput, const char* aName);
	bool GradientEditor(ColorPoint*& draggingMark, ColorPoint*& selectedMark);

private:
	void DrawGradientBar(ImVec2 const& bar_pos, float maxWidth, float height);
	void DrawGradientMarks(ColorPoint*& draggingMark, ColorPoint*& selectedMark, struct ImVec2 const& bar_pos, float maxWidth, float height);
	bool GradientButton(); 

	std::vector<ColorPoint> myColors;
};
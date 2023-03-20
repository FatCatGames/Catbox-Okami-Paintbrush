#pragma once
#include "Graphics\ColorGradient.h"

class BrushVS;
class Paintbrush : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	enum ColorMode
	{
		Black,
		Red,
		Rainbow
	} myColorMode;

	//Paint
	ColorGradient myGradient;
	Vector3f myLastPaintPos;
	Vector3f myCurrentPaintDir;
	float myLastLength = 0;
	float myTurnSmoothingSpeed = 5;

	float myPaintTimer = 0;
	float myRemainingPaint = 0;
	float myPaintDecreaseSpeed = 0.02f;
	const float myGradientSpeed = 0.01f;
	//Shape
	const float mySensitivity = 10;
	const float mySpeedScale = 1;
	const float myDensity = 0.35f;
	const float myRadius = 25;
	float myMaxSize = 40;
	float myMinSize = 2;
	int myMaxMouseDelta = 100;
	BrushVS* myShader;
};
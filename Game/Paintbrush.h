#pragma once
#include "Graphics\ColorGradient.h"

class Paintbrush : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	bool myIsGay = false;

	//Paint
	ColorGradient myGradient;
	float myPaintTimer = 0;
	float myRemainingPaint = 0;
	float myPaintDecreaseSpeed = 0.03f;
	const float myGradientSpeed = 0.01f;
	
	//Shape
	const float mySensitivity = 10;
	const float mySpeedScale = 0.5f;
	const float myDensity = 0.35f;
	const float myRadius = 30;
	float myMaxSize = 40;
	float myMinSize = 4;
	int myMaxMouseDelta = 100;
};
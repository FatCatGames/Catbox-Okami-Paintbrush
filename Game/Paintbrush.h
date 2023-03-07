#pragma once
#include "Graphics\ColorGradient.h"

class Paintbrush : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	ColorGradient myGradient;
	float myPaintTimer = 0;
	float myRemainingPaint = 0;
};
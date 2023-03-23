#pragma once
#include "CommonUtilities\Curve.h"

class FlowerTrail : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	Curve mySizeCurve;
	float myCurrentLifeTime = 0;
	float myTargetLifetime = 2;
	float mySizeMultiplier = 1;
};
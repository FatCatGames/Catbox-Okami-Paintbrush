#pragma once
#include "CommonUtilities\Curve.h"

class FlowerTrail : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	bool myHasAppliedOffset = false;
};
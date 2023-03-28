#pragma once

class ModelInstance;
class BreakableRock : public Component
{
public:
	void Update() override;
	void Break();

private:

	ModelInstance* myModel;
	bool myIsLerping = false;
	float myDeadTime = 0;
	float myLerpTimer = 0;
	float myLerpTarget = 1;
};
#pragma once

class ModelInstance;
class BreakableRock : public Component
{
public:
	void OnTriggerEnter(Collider* aCollider) override;
	void OnOverlapBegin(Collider* aCollider) override;
	void Update() override;

private:

	ModelInstance* myModel;
	bool myIsLerping = false;
	float myDeadTime = 0;
	float myLerpTimer = 0;
	float myLerpTarget = 1;
};
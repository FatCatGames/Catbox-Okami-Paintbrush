#pragma once

class BreakableRock : public Component
{
public:
	void OnTriggerEnter(Collider* aCollider) override;
	void OnOverlapBegin(Collider* aCollider) override;

private:
};
#pragma once
class Bomb : public Component
{
public:
	void OnTransformChanged() override;
	void Update() override;

private:
	Vector3f mySpawnPos;
	float myAliveTime = 0;
	bool myHasSavedPos = false;
	bool myHasLoadedPos = false;
};
#pragma once
#include "DayNightCycle.h"

class GameScene : public Component
{
public:
	static GameScene* GetInstance() { return Instance; }
	GameScene();
	void Update() override;
	void PerformAction(const std::string& anAction, Vector2i& aPosition);

private:
	static GameScene* Instance;
	DayNightCycle myDayNightCycle;
};
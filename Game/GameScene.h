#pragma once
#include "DayNightCycle.h"

class GameScene : public Component
{
public:
	static GameScene* GetInstance() { return Instance; }
	GameScene();
	void Update() override;
	void PerformAction(BrushSymbol& anAction);

private:
	static GameScene* Instance;
	DayNightCycle myDayNightCycle;
};
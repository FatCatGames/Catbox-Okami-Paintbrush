#pragma once
class DayNightCycle
{
public:
	DayNightCycle();
	void Update();
	void SetTime(bool aSetToDay);

private:
	float myLerpTimer = 2;
	float myLerpTarget = 2;
	bool myIsDay = true;
	Color myDaytimeColor;
	Color myNighttimeColor;
	Color myCurrentColor;
};
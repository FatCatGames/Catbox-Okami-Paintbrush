#pragma once

struct TimerPayload
{
	std::function<void()> aPayLoadFuction;
	float myElpsedTime = 0.0f;
	float myTime = 0.0f;
	bool myShouldUpdate = false;
};
class TimerManager
{
public:
	static bool TimerExist(int aId) { return myPayLoadMap.find(aId) != myPayLoadMap.end(); }
	static void Update(float aDeltaTime);
	static UINT AddTimer(TimerPayload aTimerPayLoad);
	static UINT AddTimer(std::function<void()> aFuction, float aCurrentTime = 0.0f, float aStartTime = 0.0f, bool aShouldUpdate = false);
	
private:
	inline static std::unordered_map<int,TimerPayload> myPayLoadMap;
	inline static int myIdCounter = -1;
};


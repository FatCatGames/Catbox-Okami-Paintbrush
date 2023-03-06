#include "stdafx.h"
#include "TimerManager.h"

void TimerManager::Update(float aDeltaTime)
{
	for (auto timer = myPayLoadMap.begin(); timer != myPayLoadMap.end();)
	{
		timer->second.myElpsedTime += aDeltaTime;
		if (timer->second.myElpsedTime >= timer->second.myTime)
		{
			timer->second.aPayLoadFuction();
			if (timer->second.myShouldUpdate)
			{
				timer->second.myElpsedTime -= timer->second.myTime;
			}
			else
			{
				timer->second.aPayLoadFuction = nullptr;
				timer = myPayLoadMap.erase(timer);
				continue;
			}
		}
		++timer;
	}
}

UINT TimerManager::AddTimer(TimerPayload aTimerPayLoad)
{
	myIdCounter++;
	myPayLoadMap.insert({ myIdCounter, aTimerPayLoad });
	return myIdCounter;
}

UINT TimerManager::AddTimer(std::function<void()> aFuction, float aCurrentTime, float aStartTime, bool aShouldUpdate)
{
	myIdCounter++;
	TimerPayload aPayLoad;
	aPayLoad.aPayLoadFuction = aFuction;
	aPayLoad.myElpsedTime = aCurrentTime;
	aPayLoad.myTime = aStartTime;
	aPayLoad.myShouldUpdate = aShouldUpdate;
	myPayLoadMap.insert({ myIdCounter, aPayLoad });
	return myIdCounter;
}

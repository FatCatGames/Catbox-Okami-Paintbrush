#include "Game.pch.h"
#include "DayNightCycle.h"

DayNightCycle::DayNightCycle()
{
	myDaytimeColor = Color(0.58f, 0.8f, 1, 1);
	myNighttimeColor = Color(0.06f, 0.06f, 0.27f, 1);
}

void DayNightCycle::Update()
{
	if (myLerpTimer < myLerpTarget)
	{
		myLerpTimer += deltaTime;
		float percent = myLerpTimer / myLerpTarget;
		if (!myIsDay)
		{
			myCurrentColor = Catbox::Lerp(myNighttimeColor, myDaytimeColor, percent);
			Engine::GetInstance()->GetMainCamera()->GetPostProcessingVolume()->SetBlendValue(1 - percent);
		}
		else
		{
			myCurrentColor = Catbox::Lerp(myDaytimeColor, myNighttimeColor, percent);
			Engine::GetInstance()->GetMainCamera()->GetPostProcessingVolume()->SetBlendValue(percent);
		}

		GraphicsEngine::GetInstance()->SetClearColor(myCurrentColor);

		if (myLerpTimer >= myLerpTarget)
		{
			myIsDay = !myIsDay;
		}
	}
}

void DayNightCycle::SetTime(bool aSetToDay)
{
	if (myIsDay != aSetToDay)
	{
		myLerpTimer = 0;
	}
}

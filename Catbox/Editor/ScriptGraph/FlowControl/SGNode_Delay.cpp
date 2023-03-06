#include "stdafx.h"
#include "SGNode_Delay.h"
#include "TimerManager.h"

void SGNode_Delay::Init()
{
	SetTitle("Delay");
	SetCategory("Flow Control");	

	CreateExecPin("In", PinDirection::Input, true);

	CreateDataPin<bool>("Looping", PinDirection::Input);
	CreateDataPin<float>("Wait Time", PinDirection::Input);
	CreateExecPin("Done", PinDirection::Output, true);

}

size_t SGNode_Delay::DoOperation()
{
	float myCurrentTime = 0.0f;
	if (GetPinData("Wait Time", myCurrentTime)) 
	{
		bool myShouldLoop = false;
		GetPinData("Looping", myShouldLoop);
		TimerManager::AddTimer([this] { ExitViaPin("Done"); }, 0.0f, myCurrentTime, myShouldLoop);
	}

	return 0;
}

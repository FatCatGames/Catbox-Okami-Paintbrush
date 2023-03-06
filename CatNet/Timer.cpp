#include "Timer.h"

Catbox::Timer::Timer()
{
	myStartTime = std::chrono::high_resolution_clock::now();
	myPreviousTime = myStartTime;
	myTimeSinceLastFrame = 0;
}

void Catbox::Timer::Update()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	auto elapsed = currentTime - myPreviousTime;
	myTimeSinceLastFrame = static_cast<float>(elapsed.count() / 10e8);
	myPreviousTime = currentTime;
}

float Catbox::Timer::GetDeltaTime() const
{
	if (myIsPaused) return 0;
	return myTimeSinceLastFrame;
}

double Catbox::Timer::GetTotalTime() const
{
	return  (std::chrono::high_resolution_clock::now() - myStartTime).count() / 10e8;
}

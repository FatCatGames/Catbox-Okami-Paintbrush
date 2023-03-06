#pragma once
#include <chrono>

namespace Catbox
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;
		void Update();
		float GetDeltaTime() const;
		double GetTotalTime() const;
		inline bool IsPaused() { return myIsPaused; }
		inline void SetPaused(bool aIsPaused) { myIsPaused = aIsPaused; }

	private:
		float myTimeSinceLastFrame;
		std::chrono::steady_clock::time_point myStartTime;
		std::chrono::steady_clock::time_point  myPreviousTime;
		bool myIsPaused = false;
	};
};
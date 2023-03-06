#pragma once
#include "Editor/Windows/EditorWindow.h"


class PerformanceWindow : public EditorWindow
{
public:


private:
	void Render() override;
	void UpdateFPS();

	float myTimer = 0;
	unsigned int myFPSCounter;
	unsigned int myFPS;
};
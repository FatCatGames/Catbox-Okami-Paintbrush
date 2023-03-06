#include "stdafx.h"
#include "WindowTemplate.h"

void WindowTemplate::Render()
{
	if (!BeginWindow("New Window", &myShouldOpen)) return;

	EndWindow();
}
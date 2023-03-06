#include "stdafx.h"
#include "PerformanceWindow.h"
#include "ImGui/imgui_stdlib.h"
#include "Graphics/Rendering/Renderers/DeferredRenderer.h"
#include "Graphics/Rendering/Renderers/ForwardRenderer.h"

void PerformanceWindow::Render()
{
	if (!BeginWindow("Performance Statistics", true)) return;
	
	UpdateFPS();

	std::string temp = "FPS : " + std::to_string(myFPS);
	ImGui::Text(&temp[0]);

	temp = "Deferred Draw Calls : " + std::to_string(Engine::GetInstance()->GetGraphicsEngine()->GetDeferredRenderer().GetDrawCalls());
	ImGui::Text(&temp.c_str()[0]);
	temp = "Forward Draw Calls : " + std::to_string(Engine::GetInstance()->GetGraphicsEngine()->GetForwardRenderer().GetDrawCalls());
	ImGui::Text(&temp.c_str()[0]);

	EndWindow();
}

void PerformanceWindow::UpdateFPS()
{
	myFPSCounter++;
	myTimer += Engine::GetInstance()->GetDeltaTime();
	if (myTimer > 1.0f) 
	{
		myTimer = 0;
		myFPS = myFPSCounter;
		myFPSCounter = 0;
	}
}

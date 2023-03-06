#include "stdafx.h"
#include "ShaderWindow.h"

void ShaderWindow::Render()
{
	ImGui::Begin("Shader Editor", &myShouldOpen);
	RenderInFrontCheck();
	
	if (ImGui::Button("Create Shader")) 
	{

	}

	ImGui::End();

	if (!myShouldOpen)
	{
		Editor::GetInstance()->GetWindowHandler().CloseWindow(this);
	}
}

#include "VisualScriptingWindow.h"
#include "stdafx.h"
#include "VisualScriptingWindow.h"
#include "Assets\Blueprint.h"
#include "MuninGraph.h"
#include "Editor/ScriptGraph/BlueprintEditor.h"


void VisualScriptingWindow::Render()
{
	if (!BeginWindow("Visual Scripting", &myShouldOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		return;
	}

	RenderInFrontCheck();
	Editor::GetInstance()->GetBlueprintEditor()->Render();
	Editor::GetInstance()->GetBlueprintEditor()->Update(deltaTime);
	//myEditor.UpdateTime(deltaTime);
	//if (Input::GetKeyPress(KeyCode::P)) 
	//{
	//	myEditor.Stop();
	//}
	EndWindow();
}

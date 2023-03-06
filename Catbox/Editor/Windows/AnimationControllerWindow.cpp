#include "stdafx.h"
#include "AnimationControllerWindow.h"
#include "MuninGraph.h"
#include "Editor/ScriptGraph/AnimationController/AnimationControllerEditor.h"


void AnimationControllerWindow::Render()
{
	if (!BeginWindow("Animation Controller Editor", true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		return;
	}

	//RenderInFrontCheck();
	Editor::GetInstance()->GetAnimationControllerEditor()->Render();
	Editor::GetInstance()->GetAnimationControllerEditor()->Update(deltaTime);
	EndWindow();
}

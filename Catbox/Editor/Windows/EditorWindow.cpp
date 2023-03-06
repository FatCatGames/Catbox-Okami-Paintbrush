#include "stdafx.h"
#include "EditorWindow.h"

void EditorWindow::RenderInFrontCheck()
{
	if (ImGui::IsWindowFocused())
	{
		if (!myIsFocused)
		{
			myIsFocused = true;
			Editor::GetInstance()->GetWindowHandler().BringToFront(this);
		}
	}
	else myIsFocused = false;
}

bool EditorWindow::BeginWindow(const char* aWindowName, bool aCanBeClosed, ImGuiWindowFlags flags)
{
	if (!ImGui::Begin(aWindowName, aCanBeClosed ? &myShouldOpen : 0, flags))
	{
		myIsOpen = false;
		ImGui::End();
		if (!myShouldOpen)
		{
			Editor::GetInstance()->GetWindowHandler().CloseWindow(this);
		}
		return false;
	}
	myIsOpen = true;
	RenderInFrontCheck();
	return true;
}

void EditorWindow::EndWindow()
{
	ImGui::End();
}
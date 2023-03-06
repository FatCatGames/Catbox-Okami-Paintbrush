#include "stdafx.h"
#include "HotKeyWindow.h"
#include "ProjectSettings.h"
void HotKeyWindow::OrderNames(const std::unordered_map<std::string, KeyCode>& aMap)
{
	myOrderedNames.clear();
	for (auto& key : aMap)
	{
		myOrderedNames.push_back(key.first);
	}
	std::sort(myOrderedNames.begin(), myOrderedNames.end());
	ProjectSettings::SaveKeyBinds();
}
void HotKeyWindow::Render()
{
	if (!BeginWindow("HotKey Window", true)) return;
	if (!myFirstRun) 
	{
		myFirstRun = true;
		OrderNames(Input::GetControllerScheme());
	}
	auto& hotkeys = Input::GetControllerScheme();
	for (auto key : myOrderedNames)
	{
		ImGui::Text(key.c_str());
		ImGui::SameLine();
		ImGui::Text((Input::GetKeyName(hotkeys.find(key)->second)).c_str());
		ImGui::SameLine(ImGui::GetWindowSize().x - 115);
		if (myCurrentEditHotkey != key.c_str())
		{
			if (ImGui::Button(("Change Key##" + key).c_str()))
			{
				myCurrentEditHotkey = key;
			}
		}
		else 
		{
			ImGui::Text("Recording Key");
		}
		ImGui::SameLine(ImGui::GetWindowSize().x - 20);
		if(ImGui::Button("X"))
		{
			Input::RemoveHotKey(key);
			OrderNames(Input::GetControllerScheme());
		}
	}
	if (!myAddButtonClicked) 
	{
		if (ImGui::Button("Add"))
		{
			myAddButtonClicked = true;
		}
	}
	else
	{
		ImGui::SetKeyboardFocusHere(0);
		ImGui::InputText("Name", &myAddKey);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Input::AddKeyBind(myAddKey, KeyCode::COUNT);
			OrderNames(Input::GetControllerScheme());
			myAddKey = "";
			myAddButtonClicked = false;
		}
	}
	KeyCode aKeyCode = Input::GetCurrentKeyPressed();
	if (myCurrentEditHotkey != "" && aKeyCode != KeyCode::COUNT)
	{	
		Input::AddKeyBind(myCurrentEditHotkey, aKeyCode);
		myCurrentEditHotkey = "";
		ProjectSettings::SaveKeyBinds();
	}

	EndWindow();
}


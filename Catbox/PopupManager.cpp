#include "stdafx.h"
#include "PopupManager.h"
#include "Editor/Windows/SceneWindow.h"

std::vector<Popup> PopupManager::myPopups;

void PopupManager::CreatePopup(std::string aName, float aTime, Vector2f aSize, std::string aContent, Vector2f aPos)
{
	for (int i = 0; i < myPopups.size(); i++)
	{
		if(aName == myPopups[i].name) 
		{
			return;
		}
	}
	Popup aPopup;
	aPopup.aliveTime = 0;
	aPopup.maxAliveTime = aTime;
	aPopup.name = aName;
	aPopup.size = aSize;
	aPopup.content = aContent;
	aPopup.pos = aPos;
	myPopups.push_back(aPopup);
}

void PopupManager::Render()
{
	auto viewPort = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene);
	for (auto& popup : myPopups)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, (0.8f * (popup.maxAliveTime - popup.aliveTime))));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0.8f * (popup.maxAliveTime - popup.aliveTime)));
		auto currentWindowSize = ImVec2(popup.size.x * viewPort->GetSize().x / Engine::GetInstance()->GetWindowSize().x, popup.size.y * viewPort->GetSize().y / Engine::GetInstance()->GetWindowSize().y);
		ImGui::SetNextWindowSize(currentWindowSize);

		Vector2f pos = { (viewPort->GetPosition().x) + viewPort->GetSize().x * 0.5f, (viewPort->GetPosition().y) + viewPort->GetSize().y * 0.5f };
		ImGui::SetNextWindowPos({ pos.x - (currentWindowSize.x * 0.5f), pos.y - (currentWindowSize.y * 0.5f) - (popup.pos.y * (currentWindowSize.y / popup.size.y))});

		if (ImGui::Begin(popup.name.c_str(),0,ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			if (popup.content.size() > 0) 
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.8f * (popup.maxAliveTime - popup.aliveTime)));
				ImGui::SetWindowFontScale( 2.0f * (currentWindowSize.x /popup.size.x));
				ImGui::CenteredText(popup.content.c_str(), {ImGui::GetWindowWidth() - (17 * (currentWindowSize.x / popup.size.x)), ImGui::GetWindowHeight() - (20 * (currentWindowSize.y / popup.size.y))});
				ImGui::PopStyleColor();
			}
			ImGui::End();
		}
	}
}

void PopupManager::Update()
{
	for (int i = myPopups.size() -1 ; i >= 0; i--)
	{
		myPopups[i].aliveTime += deltaTime;
		if (myPopups[i].aliveTime >= myPopups[i].maxAliveTime)
		{
			myPopups.erase(myPopups.begin() + i);
		}
	}
}

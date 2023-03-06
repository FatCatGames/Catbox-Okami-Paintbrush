#include "stdafx.h"
#include "ConsoleWindow.h"
#include "CommonUtilities/InputHandler.h"

void ClearMessages() 
{
	MessageHandler::debugMessages.myMessagesAmmount.clear();
	MessageHandler::debugMessages.messages.clear();
}

void ClearErrors()
{
	MessageHandler::debugMessages.errors.clear();
}

void PrintMessages(std::vector<string>& someMessages)
{
	for (int i = static_cast<int>(someMessages.size()) -1; i >= 0; i--)
	{
		ImGui::Text(someMessages[i].c_str());
		if (MessageHandler::debugMessages.myCollapseToggle) 
		{
			if (MessageHandler::debugMessages.myMessagesAmmount[someMessages[i]] > 1)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.25f, 1, 1));
				ImGui::SameLine(ImGui::GetWindowSize().x - 60.0f);
				ImGui::Text(std::to_string(MessageHandler::debugMessages.myMessagesAmmount[someMessages[i]]).c_str());
				ImGui::PopStyleColor();
			}
		}
	}
}

void PrintErrors(std::vector<string>& someErrors)
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255));
	for (int i = static_cast<int>(someErrors.size()) -1; i >= 0; i--)
	{
		ImGui::Text(someErrors[i].c_str());
	}
	ImGui::PopStyleColor();
}

void ConsoleWindow::Render()
{
	if (!BeginWindow("Console", true)) return;

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("DebugConsole", tab_bar_flags))
	{
		auto& messages = MessageHandler::debugMessages.messages;
		auto& errors = MessageHandler::debugMessages.errors;
		if (ImGui::BeginTabItem("All"))
		{
			if (ImGui::Button("Clear")) 
			{
				ClearMessages();
				ClearErrors();
			}
			if (ImGui::Checkbox("Collapse Prints", &myCollapseToggle))
			{
				MessageHandler::debugMessages.myCollapseToggle = myCollapseToggle;
			}
			PrintMessages(messages);
			PrintErrors(errors);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Messages"))
		{
			if (ImGui::Button("Clear"))
			{
				ClearMessages();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Collapse Prints", &myCollapseToggle))
			{
				MessageHandler::debugMessages.myCollapseToggle = myCollapseToggle;
			}
			PrintMessages(messages);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Errors"))
		{
			if (ImGui::Button("Clear"))
			{
				ClearErrors();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Collapse Prints", &myCollapseToggle))
			{
				MessageHandler::debugMessages.myCollapseToggle = myCollapseToggle;
			}
			PrintErrors(errors);
			ImGui::EndTabItem();
		}
		
		ImGui::EndTabBar();
	}

	EndWindow();
}



#include "stdafx.h"
#include "ChatWindow.h"
#include "Networking\Client.h"

const int charLimit = 500;
const char* hex_digits = "0123456789ABCDEF";
std::string RGBToHex(int r, int g, int b) {
	std::string hexValue;
	hexValue.resize(6);
	hexValue[0] = hex_digits[r / 16];
	hexValue[1] = hex_digits[r % 16];
	hexValue[2] = hex_digits[g / 16];
	hexValue[3] = hex_digits[g % 16];
	hexValue[4] = hex_digits[b / 16];
	hexValue[5] = hex_digits[b % 16];
	return hexValue;
}

void HexToRGB(char* hex, int& r, int& g, int& b) {
	sscanf_s(hex, "%02x%02x%02x", &r, &g, &b);
}

ChatWindow::ChatWindow()
{
	myIP = Editor::GetInstance()->GetUserPrefs().myDefaultIP;
	myUsername = Editor::GetInstance()->GetUserPrefs().myUsername;
	int r, g, b;
	HexToRGB(&Editor::GetInstance()->GetUserPrefs().myChatColor[0], r, g, b);
	myColor[0] = r / 255.f;
	myColor[1] = g / 255.f;
	myColor[2] = b / 255.f;
}

void ChatWindow::Connect()
{
	if (myUsername.empty())
	{
		myConnectionFailed = true;
		return;
	}

	if (myUsername.size() > myUserNameLimit)
	{
		myConnectionFailed = true;
		return;
	}

	myClient = Engine::GetInstance()->GetNetworkManager()->Connect(myUsername, myIP, 42000);
	if (!myClient)
	{
		myConnectionFailed = true;
		return;
	}

	myIsConnected = true;
}

void ChatWindow::Send()
{
	//myMessageHistory.push_back(myMessage);
	Engine::GetInstance()->GetNetworkManager()->SendString(myMessage);
	myMessage.clear();
}



void ChatWindow::Render()
{
	if (!BeginWindow("Chat", &myShouldOpen)) return;

	if (!myIsConnected)
	{
		if (myConnectionFailed)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text("Failed to connect. ");
			if (myUsername.empty())
			{
				ImGui::SameLine();
				ImGui::Text("Input username.");
			}
			else if (myUsername.size() > myUserNameLimit)
			{
				ImGui::SameLine();
				ImGui::Text("Too long username.");
			}
			ImGui::PopStyleColor();
		}
		bool popColor = false;
		if (myUsername.size() > myUserNameLimit)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			popColor = true;
		}

		ImGui::InputText("Username", &myUsername);

		if (popColor)
		{
			ImGui::PopStyleColor();
		}

		ImGui::InputText("IP", &myIP);

		if (ImGui::Button("Connect"))
		{
			Connect();
			Editor::GetInstance()->GetUserPrefs().myUsername = myUsername;
			Editor::GetInstance()->GetUserPrefs().myDefaultIP = myIP;
			Editor::GetInstance()->GetUserPrefs().SaveUserPreferences();
			if (myClient)
			{
				myClient->SetColor(Editor::GetInstance()->GetUserPrefs().myChatColor);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset to default"))
		{
			myIP = Editor::GetInstance()->GetUserPrefs().myDefaultIP;
		}
	}
	else
	{
		myChatMessages = Engine::GetInstance()->GetNetworkManager()->GetChatMessage();

		ImGui::Text("Connected to CatNet.");

		if (myChatMessages.size() != myMessageCountLastFrame)
		{
			myMessageCountLastFrame = myChatMessages.size();
			myShouldResetScroll = true;
		}
		for (size_t i = 0; i < myChatMessages.size(); i++)
		{
			if (myChatMessages[i].GetMessageType() == MessageType::ChatMessage)
			{
				int r, g, b;
				HexToRGB(myChatMessages[i].GetColor(), r, g, b);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r / 255.f, g / 255.f, b / 255.f, 1));
				ImGui::Text(myChatMessages[i].GetSender());
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text(":");
				ImGui::SameLine();
				ImGui::TextWrapped(myChatMessages[i].GetBuffer());
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
				ImGui::Text(myChatMessages[i].GetBuffer());
				ImGui::PopStyleColor();
			}

		}

		ImGui::Text("Say:");
		ImGui::SameLine();
		if (myShouldResetFocus)
		{
			myShouldResetFocus = false;
			ImGui::SetKeyboardFocusHere();
		}

		bool popColor = false;
		if (myMessage.size() > charLimit)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			popColor = true;
		}

		ImGui::InputText("##ChatInput", &myMessage);

		if (popColor)
		{
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemDeactivated())
		{
			if (Input::GetKeyPress(KeyCode::RETURN))
			{
				myShouldResetFocus = true;
				if (!myMessage.empty())
				{
					Send();
					myShouldResetScroll = true;
				}
			}
		}

		if (myShouldResetScroll)
		{
			myShouldResetScroll = false;
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::SameLine();
		ImGui::ColorEdit3("##UserColor", myColor, ImGuiColorEditFlags_NoInputs);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			const unsigned int r = (static_cast<unsigned int>(myColor[0] * 255));
			const unsigned int g = (static_cast<unsigned int>(myColor[1] * 255));
			const unsigned int b = (static_cast<unsigned int>(myColor[2] * 255));

			myClient->SetColor(RGBToHex(r, g, b));
			Editor::GetInstance()->GetUserPrefs().myChatColor = myClient->GetColor();
			Editor::GetInstance()->GetUserPrefs().SaveUserPreferences();
		}
	}

	EndWindow();
}
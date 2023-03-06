#pragma once
#include "EditorWindow.h"
#include "Networking/NetworkManager.h"

namespace CatNet
{
	class Client;
}
class ChatWindow : public EditorWindow
{
public:
	ChatWindow();


private:
	void Connect();
	void Send();
	void Render() override;
	bool myIsConnected = false;
	bool myConnectionFailed = false;
	bool myShouldResetFocus = false;
	bool myShouldResetScroll = false;
	size_t myMessageCountLastFrame = 0;
	int myUserNameLimit = 20;
	std::string myMessage;
	std::string myUsername = "";
	std::string myIP = "127.0.0.1";
	//std::string myPort = "42000";
	float myColor[3] = { 1,1,1 };
	std::vector<ChatMessage> myChatMessages;
	std::shared_ptr<CatNet::Client> myClient;

};


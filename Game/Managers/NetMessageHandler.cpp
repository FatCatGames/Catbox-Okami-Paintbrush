#include "Game.pch.h"
#include "NetMessageHandler.h"
#include "Networking\NetworkManager.h"

NetMessageHandler* NetMessageHandler::Instance;

NetMessageHandler::NetMessageHandler()
{
	if (Instance) return;
	Instance = this;
	myMessageListener.action = [this](void* data) {HandleMessage(reinterpret_cast<NetMessagePayload*>(data)); };
	myNetworkManager = Engine::GetInstance()->GetNetworkManager();
	myNetworkManager->RegisterMessageListener(myMessageListener);
}

void NetMessageHandler::SendNetMessage(NetMessagePayload& aPayload)
{
	std::string printMsg = "Sent message of type ";
	printMsg += std::to_string(static_cast<int>(aPayload.messageType));
	//print(printMsg);
	myNetworkManager->SendNetMessage(aPayload);
}

void NetMessageHandler::HandleMessage(NetMessagePayload* aPayload)
{
	std::string printMsg = "Received message of type ";
	printMsg += std::to_string(static_cast<int>(aPayload->messageType));
	//print(printMsg);
}

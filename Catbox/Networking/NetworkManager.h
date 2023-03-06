#pragma once
#include "Client.h"
#include "ComponentTools\Event.h"
#include <mutex>


namespace CatNet 
{
	class NetworkManager
	{
	public:
		NetworkManager() = default;
		~NetworkManager();
		std::shared_ptr<Client> Connect(const std::string& aUserName, const std::string& aIp, int aPort);
		void Disconnect();
		bool SendString(const std::string& aMessage);
		void SendNetMessage(NetMessagePayload& aPayload);
		void Update();
		unsigned short GetClientId() { return myClient->GetId(); }
		std::vector<ChatMessage>& GetChatMessage();
		void ReceiveMessage(NetMessage* aMessage, MessageType aMessageType);
		void RegisterMessageListener(ListenerWithParameter& aListener);
		void DeRegisterMessageListener(ListenerWithParameter& aListener);
		int GetNextId() { return myNetworkIDs++; }

	private:
		std::shared_ptr<Client> myClient = nullptr;
		std::vector<std::string> myMessageHistory;
		Event myOnMessageReceivedEvent;
		std::vector<NetMessagePayload> myPayloads;
		int myNetworkIDs = 0;
		std::mutex myGameMessageLock;
	};
}


#pragma once
#include "NetMessagePayload.h"
#include <iostream>
#include <string.h>
#include <WinSock2.h>
#include "ws2tcpip.h"
#include <vector>
#include <mutex>


namespace CatNet
{
	const size_t MessageBuffer = 512;
	const int DefaultPort = 20000;

	class Client
	{
	public:
		bool Init(const std::string& aUserName, const std::string& aIp, int aPort);
		inline void SetColor(std::string aHEX) { myHEX = aHEX; }
		const std::string& GetColor() { return myHEX; }
		const std::string& GetName() { return myUserName; }
		void DeInit();
		void SendMessageLoop();
		void ReciveMessageLoop();
		std::vector<ChatMessage>& GetChatMessages();
		void AddMessage(NetMessage* aMessage);
		unsigned short GetId() { return myId; }
		void PrintLastError()
		{
			int error = WSAGetLastError();
			std::cout << "Error " << error << std::endl;
		}
		inline bool IsConnected() { return myIsConnected; }

	private:
		unsigned int myId = 0;
		bool myIsConnected = false;
		int myMessageCounter = 0;
		std::string myUserName = "None";
		std::string myHEX = "FFFFFF";
		SOCKET mySocket{};
		WSADATA mySocketData{};
		sockaddr_in newFromAddress{};
		sockaddr_in myServerAddress{ 0 };
		std::vector<NetMessage*> myNetMessages;
		std::vector<ChatMessage> myReceivedMessages;

		std::mutex mySendMessageLock;

		char messageBuffer[CatNet::MessageBuffer]{ "Connect" };
		char recivedBuffer[CatNet::MessageBuffer]{ };
	};
}
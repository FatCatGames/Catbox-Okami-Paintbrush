#include "stdafx.h"
#include "Client.h"
#include "NetworkManager.h"

using namespace CatNet;


bool Client::Init(const std::string& aUserName, const std::string& aIp, int aPort)
{
	if (WSAStartup(MAKEWORD(2, 2), &mySocketData) != 0)
	{
		PrintLastError();
	}
	myUserName = aUserName;

	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mySocket == INVALID_SOCKET)
	{
		PrintLastError();
	}

	myServerAddress.sin_family = AF_INET;

	int timeoutLimit = 2000; //ms
	setsockopt(mySocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutLimit, sizeof(timeoutLimit));

	while (!myIsConnected)
	{
		memset(messageBuffer, '\0', CatNet::MessageBuffer);

		//converts an ip string to binary representation to be able to store it in a sin_addr.
		inet_pton(AF_INET, aIp.c_str(), &myServerAddress.sin_addr.s_addr);

		myServerAddress.sin_port = htons(aPort);


		ConnectMessage aNetMessage;

		aNetMessage.SetMessage({ myUserName });

		memcpy_s(messageBuffer, CatNet::MessageBuffer, &aNetMessage, CatNet::MessageBuffer);

		const auto result = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}
		else
		{
			memset(messageBuffer, '\0', CatNet::MessageBuffer);
			int fromSize = sizeof(myServerAddress);
			const auto receivedSize = recvfrom(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), &fromSize);
			if (receivedSize == SOCKET_ERROR)
			{
				PrintLastError();
				return false;
				break;
			}
			else
			{
				myId = int(messageBuffer[0]);
				myIsConnected = true;
			}
		}
		u_long ne = TRUE;

		ioctlsocket(mySocket, FIONBIO, &ne);
		return true;
	}
}

void Client::DeInit()
{
	DisconnectMessage msg;
	msg.SetSenderId(myId);

	memcpy_s(messageBuffer, CatNet::MessageBuffer, &msg, CatNet::MessageBuffer);

	const auto newResult = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));
	if (newResult == SOCKET_ERROR)
	{
		PrintLastError();
	}

	closesocket(mySocket);
	WSACleanup();
	myIsConnected = false;
}

void Client::SendMessageLoop()
{
	for (int i = 0; i < myNetMessages.size(); i++)
	{
		memset(messageBuffer, '\0', CatNet::MessageBuffer);

		if (myNetMessages[i]->GetMessageType() == MessageType::ChatMessage)
		{
			ChatMessage aChatMessage = *reinterpret_cast<ChatMessage*>(myNetMessages[i]);

			memcpy_s(messageBuffer, CatNet::MessageBuffer, &aChatMessage, CatNet::MessageBuffer);

			const auto newResult = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));
			if (newResult == SOCKET_ERROR)
			{
				PrintLastError();
			}
		}
		else if (myNetMessages[i]->GetMessageType() == MessageType::GameMessage) 
		{
			GameMessage* aGameMessage = reinterpret_cast<GameMessage*>(myNetMessages[i]);
			
			memcpy_s(messageBuffer, CatNet::MessageBuffer, aGameMessage, aGameMessage->GetMessageSize());

			const auto newResult = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));
			if (newResult == SOCKET_ERROR)
			{
				PrintLastError();
			}
		}
		else if (myNetMessages[i]->GetMessageType() == MessageType::RecivedGuaranteedMessage)
		{
			//print("Sent validation message id: " + std::to_string(myNetMessages[i]->GetGuaranteedId()));
			GuarateedMessageValidate* aGameMessage = reinterpret_cast<GuarateedMessageValidate*>(myNetMessages[i]);

			memcpy_s(messageBuffer, CatNet::MessageBuffer, aGameMessage, sizeof(NetMessage));

			const auto newResult = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));
			if (newResult == SOCKET_ERROR)
			{
				PrintLastError();
			}
		}
	}
	for (int i = 0; i < myNetMessages.size(); i++)
	{
		if (myNetMessages[i]) 
		{
  			delete myNetMessages[i]; 
		}
	}
	myNetMessages.clear();
}

void Client::ReciveMessageLoop()
{
	while (myIsConnected)
	{
		int fromSize = sizeof(myServerAddress);
		const auto receivedSize = recvfrom(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), &fromSize);
		if (receivedSize == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			PrintLastError();
		}
		if (receivedSize > 0)
		{
			//char address[16]{ 0 };
			//inet_ntop(AF_INET, &newFromAddress.sin_addr, &address[0], 16);
			//const auto fromPort = ntohs(newFromAddress.sin_port);
			//std::cout << address;
			//std::cout << " : ";
			//std::cout << messageBuffer << std::endl;
			//std::cout << "Packet received from " << address << " : " << fromPort << std::endl;

			auto msgType = static_cast<MessageType>(messageBuffer[0]);

			NetMessage* tempNetMessage = reinterpret_cast<NetMessage*>(messageBuffer);
			if (tempNetMessage->GetGuaranteed())
			{
				GuarateedMessageValidate* validation = new GuarateedMessageValidate();
				validation->SetGuaranteedId(tempNetMessage->GetGuaranteedId());
				AddMessage(validation);
			}

			if (msgType == MessageType::ChatMessage || msgType == MessageType::ServerMessage)
			{
				ChatMessage* aChatMessage = reinterpret_cast<ChatMessage*>(&messageBuffer);
				myReceivedMessages.push_back(*aChatMessage);
				if (myReceivedMessages.size() > 50)
				{
					myReceivedMessages.erase(myReceivedMessages.begin());
				}
			}
			else if (msgType == MessageType::PingMessage)
			{
				memset(messageBuffer, 0, CatNet::MessageBuffer);
				PingMessage* aPingMessage = new PingMessage();
				aPingMessage->SetSenderId(myId);
				aPingMessage->SetTargetId(0);

				memcpy_s(messageBuffer, CatNet::MessageBuffer, aPingMessage, sizeof(aPingMessage));

				const auto newResult = sendto(mySocket, messageBuffer, CatNet::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myServerAddress), sizeof(myServerAddress));

				delete aPingMessage;
			}
			else if (static_cast<MessageType>(messageBuffer[0]) == MessageType::GameMessage)
			{
				void* tempGameMessage = new size_t[512];
				memcpy_s(tempGameMessage, size_t(512), messageBuffer, size_t(512));
				Engine::GetInstance()->GetNetworkManager()->ReceiveMessage(reinterpret_cast<NetMessage*>(tempGameMessage), msgType);
			}
		}
	}
}

void Client::AddMessage(NetMessage* aMessage)
{
	aMessage->SetSenderId(myId);
	aMessage->SetTargetId(0);
	aMessage->SetId(myMessageCounter++);
	myNetMessages.push_back(aMessage);
}

std::vector<ChatMessage>& Client::GetChatMessages()
{
	return myReceivedMessages;
}
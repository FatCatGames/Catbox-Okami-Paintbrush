#include "stdafx.h"
#include "NetworkManager.h"
#include "ComponentTools/ThreadPool.h"
#include <memory>
using namespace CatNet;
NetworkManager::~NetworkManager()
{
	Disconnect();
}

std::shared_ptr<Client> NetworkManager::Connect(const std::string& aUserName, const std::string& aIp, int aPort)
{
	if (!myClient)
	{
		myClient = std::make_shared<Client>();
	}
	else if(myClient->IsConnected())
	{
		return myClient;
	}

	if (myClient->Init(aUserName, aIp, aPort))
	{
		Engine::GetInstance()->GetThreadPool()->QueueJob([&] {myClient->ReciveMessageLoop(); });
		return myClient;
	}
	return nullptr;
}

void NetworkManager::Disconnect()
{
	if (myClient)
	{
		myClient->DeInit();
		myClient.reset();
	}
}


bool NetworkManager::SendString(const std::string& aMessage)
{
	int aSize = ChatMessage::GetMessageSize() - 1;
	if (myClient)
	{
		int aMinValue = 0;
		int aMaxValue = 0;
		int aMessageCount = aMessage.size();
		do
		{
			if (aMessageCount > aSize)
			{
				aMaxValue += aSize;
			}
			else
			{
				aMaxValue = aMessage.size();
			}

			std::string aNewMessage = std::string(aMessage.begin() + aMinValue, aMessage.begin() + aMaxValue);
			ChatMessage* aChatMessage = new ChatMessage();
			aChatMessage->SetMessage(aNewMessage);
			aChatMessage->SetSender(myClient->GetName());
			aChatMessage->SetColor(myClient->GetColor());
			myClient->AddMessage(aChatMessage);

			aMessageCount -= aSize;

			aMinValue += aSize;
		} while (aMessageCount > 0);
		return true;
	}
	printerror("NO CLIENT");
	return false;
}

void NetworkManager::SendNetMessage(NetMessagePayload& aPayload)
{
	myClient->AddMessage(aPayload.message);
}

void CatNet::NetworkManager::Update()
{
	if (myClient) 
	{
		myClient->SendMessageLoop();

		std::scoped_lock<std::mutex> guard(myGameMessageLock);
		std::vector<NetMessagePayload> thisFramePayloads = myPayloads;
		myPayloads.clear();
		for (int i = 0; i < thisFramePayloads.size(); i++)
		{
			myOnMessageReceivedEvent.Invoke(&thisFramePayloads[i]);
			thisFramePayloads[i].DeleteMessage();
		}
	}
}

std::vector<ChatMessage>& CatNet::NetworkManager::GetChatMessage()
{
	return myClient->GetChatMessages();
}


void CatNet::NetworkManager::ReceiveMessage(NetMessage* aMessage, MessageType aMessageType)
{
	std::scoped_lock<std::mutex> guard(myGameMessageLock);
	NetMessagePayload tempPayload;

	auto temp = static_cast<GameMessage*>(aMessage);

	tempPayload.message = aMessage;
	tempPayload.messageType = aMessageType;
	myPayloads.push_back(tempPayload);
}

void CatNet::NetworkManager::RegisterMessageListener(ListenerWithParameter& aListener)
{
	myOnMessageReceivedEvent.AddListener(aListener);
}
void CatNet::NetworkManager::DeRegisterMessageListener(ListenerWithParameter& aListener)
{
	myOnMessageReceivedEvent.RemoveListener(aListener);
}

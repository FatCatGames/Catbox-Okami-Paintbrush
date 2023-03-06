#include "Server.h"
#include "NetworkGameCommands.h"
#include "Quake.h"
Server::Server()
{
	mySocket = SOCKET{};
	mySocketData = WSADATA{};

	myAddress = sockaddr_in{};

	inet_pton(AF_INET, "10.250.224.168", &myAddress.sin_addr.s_addr);


	char messageBuffer[Net::MessageBuffer];

	std::cout << "Welcome to CatNet!" << std::endl;
	std::cout << "                         _ " << std::endl;
	std::cout << "  ,-.       _,---._ __  / \\" << std::endl;
	std::cout << " /  )    .-'       `./ /   \\" << std::endl;
	std::cout << "(  (   ,'            `/    /|" << std::endl;
	std::cout << " \\  `-'             \\'\\   / |" << std::endl;
	std::cout << "  `.              ,  \\ \\ /  |" << std::endl;
	std::cout << "   /`.          ,'-`----Y   |" << std::endl;
	std::cout << "  (            ;        |   /" << std::endl;
	std::cout << "  |  ,-.    ,-'         |  /" << std::endl;
	std::cout << "  |  | (   |  StupidBox | /" << std::endl;
	std::cout << "  )  |  \\  `.___________|/" << std::endl;
	std::cout << "  `--'   `--'              " << std::endl;

	//Starts up the winsock library. This must be called before any other winsock functions are used
	if (WSAStartup(MAKEWORD(2, 2), &mySocketData) != 0)
	{
		PrintLastError();
	}



	/*
	The first argument, AF_INET, specifies the address family, which is IPv4.
	The second argument, SOCK_DGRAM, specifies the socket type, which is a datagram socket, meaning that the socket provides a connectionless, unreliable data transfer service.
	SOCK_DGRAM is the socket type used for UDP. For TCP, use SOCK_STREAM and IPPROTO_TCP instead.
	*/
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (mySocket == INVALID_SOCKET)
	{
		PrintLastError();
	}

	//u_long mode = 1;  // 1 to enable non-blocking socket
	//ioctlsocket(mySocket, FIONBIO, &mode);

	myAddress.sin_family = AF_INET;

	char address[16]{ 0 };
	inet_ntop(AF_INET, &myAddress.sin_addr, &address[0], 16);
	std::cout << "Connected to: " << address << std::endl;

	u_long ne = TRUE;

	ioctlsocket(mySocket, FIONBIO, &ne);
}

void Server::PrintLastError()
{
	int error = WSAGetLastError();
	std::cout << "Error " << error << std::endl;
}

void Server::UpdateTimer()
{
	myTimer.Update();
	float aDelta = myTimer.GetDeltaTime();
	for (auto& timeOutTimer : myTimeOutTimers)
	{
		timeOutTimer.second -= aDelta;

		if (timeOutTimer.second < 0 && timeOutTimer.second > -1)
		{
			timeOutTimer.second = -1;
			PingUser(timeOutTimer.first);
		}
		else if (timeOutTimer.second < -myTimeOutMax)
		{
			//DisconnectUser(timeOutTimer.first);
		}
	}
	std::vector<unsigned short> removeTimerIds;
	std::vector<unsigned short> resendMessagesIds;
	for (auto& messageTimers : myGuaranteedMessagesTimers)
	{
		if (myGuaranteedGameMessages[messageTimers.first]) 
		{
			if (messageTimers.second > 12.0f)
			{
				auto messagePtr = myGuaranteedGameMessages[messageTimers.first];
				DisconnectUser(messagePtr->GetTargetId());
				removeTimerIds.push_back(messageTimers.first);
			}
			else if (messageTimers.second > 10.0f && messageTimers.second < 11.0f)
			{
				messageTimers.second = 11.0f;
				resendMessagesIds.push_back(messageTimers.first);
			}
			else if (messageTimers.second > 7.0f && messageTimers.second < 8.0f)
			{
				messageTimers.second = 8.0f;
				resendMessagesIds.push_back(messageTimers.first);
			}
			else if (messageTimers.second > 2.0f && messageTimers.second < 3.0f)
			{
				messageTimers.second = 3.0f;
				resendMessagesIds.push_back(messageTimers.first);
			}

			messageTimers.second += myTimer.GetDeltaTime();
		}
		else 
		{
			removeTimerIds.push_back(messageTimers.first);
		}
	}

	for (int i = 0; i < resendMessagesIds.size(); i++)
	{
		auto messagePtr = myGuaranteedGameMessages[resendMessagesIds[i]];
		SendGameMessage(messagePtr, messagePtr->GetTargetId(), messagePtr->GetSenderId(), true);
		myLostPacketsList[messagePtr->GetTargetId()]++;
	}
	std::vector<short> myThisFrameIds = myGuaranteedRemovedIDs;
	myGuaranteedRemovedIDs.clear();

	for (int i = 0; i < removeTimerIds.size(); i++)
	{
		myGuaranteedMessagesTimers.erase(removeTimerIds[i]);
		myGuaranteedGameMessages.erase(removeTimerIds[i]);
	}
}

void Server::UpdateGame() 
{
	for (auto& room : myRooms)
	{
		auto game = room.second.GetGame();
		if (game) 
		{
			if (game->Running())
			{
				game->Update();
			}
			else if(room.second.IsFull())
			{
				room.second.GetGame()->Start(room.second.myPlayerIds);
			}
			myGameMessages = game->GetMessages();
			for (int i = 0; i < myGameMessages.size(); i++)
			{
				for (auto& players : room.second.myPlayerIds)
				{
					SendGameMessage(myGameMessages[i], players, myGameMessages[i]->GetSenderId(), myGameMessages[i]->GetGuaranteed());
				}
			}
			game->ClearMessages();
		}
	}
	myGameMessages.clear();
}

void Server::Listen()
{
	sockaddr_in fromAddress{};
	int fromSize = sizeof(fromAddress);
	const auto recivedSize = recvfrom(mySocket, aBuffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&fromAddress), &fromSize);
	if (recivedSize == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		PrintLastError();
	}
	if (recivedSize != SOCKET_ERROR && aBuffer)
	{

		ProcessMessage(aBuffer, fromAddress);
	}
}

void Server::SendServerMessage(const std::string& aMessage)
{
	ServerMessage* serverMessage = new ServerMessage();
	serverMessage->SetMessage(aMessage);
	serverMessage->SetId(serverMessage->GetId());
	serverMessage->SetSenderId(0);

	char buffer[Net::MessageBuffer];
	memset(buffer, 0, Net::MessageBuffer);
	memcpy_s(buffer, Net::MessageBuffer, serverMessage, Net::MessageBuffer);

	for (auto& client : myClientAddress)
	{
		serverMessage->SetTargetId(client.first);
		const auto result = sendto(mySocket, buffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&client.second), sizeof(client.second));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}

		//opposite of inet_pton, converts an ip adress from binary format to a readable format.
		char address[16]{ 0 };
		inet_ntop(AF_INET, &client.second.sin_addr, &address[0], 16);
		const auto fromPort = ntohs(client.second.sin_port);
		//Change to UserName
		std::cout << serverMessage->GetBuffer() << std::endl;
		//std::cout << "Packet received from " << address << " : " << fromPort << std::to_string(myLostPacketsList[client.first] / myTotalPacketsList[client.first] * 100).c_str() << "% Packet loss so far" << std::endl;
	}

	delete serverMessage;
}

void Server::DisconnectUser(const short& aId)
{
	DisconnectMessage* disconnectMessage = new DisconnectMessage();
	disconnectMessage->SetMessage("Disconnect");
	disconnectMessage->SetId(0);
	disconnectMessage->SetTargetId(aId);
	disconnectMessage->SetSenderId(0);

	char buffer[Net::MessageBuffer];
	memset(buffer, 0, Net::MessageBuffer);
	memcpy_s(buffer, Net::MessageBuffer, disconnectMessage, Net::MessageBuffer);

	const auto result = sendto(mySocket, buffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myClientAddress[aId]), sizeof(myClientAddress[aId]));

	std::cout << "Disconnected " << myClientNames[aId] << std::endl;

	for (auto room : myRooms)
	{
		for (int i = 0; i < room.second.myPlayerIds.size(); i++)
		{
			room.second.myPlayerIds.erase(room.second.myPlayerIds.begin() + i);
		}
	}

	myClientAddress.erase(aId);
	myClientNames.erase(aId);
	myTimeOutTimers.erase(aId);

	delete disconnectMessage;
}

unsigned short Server::CreateRoom(const std::string& aRoomName, const std::string& aGameName)
{
	Room aRoom;
	aRoom.SetRoomId(++myRoomIdCounter);
	aRoom.mySlots = 1;
	Quake* aQuakeGame = new Quake();
	aRoom.SetGame(aQuakeGame);
	myRoomNames.insert({ aRoom.GetId(), aRoomName });
	myRooms.insert({ myRoomIdCounter, aRoom });
	return aRoom.GetId();
}

void Server::DisconnectAllUsers()
{
	for (auto& client : myClientAddress)
	{
		DisconnectUser(client.first);
	}
}

void Server::SendClientMessage(ChatMessage* aNetMessage, const short aTargetId, const short aSenderId, bool aGuaranteed)
{
	char buffer[Net::MessageBuffer];
	memset(buffer, 0, Net::MessageBuffer);
	std::string aMessage = aNetMessage->GetBuffer();

	int aSize = aNetMessage->GetMessageSize();
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
		memset(buffer, 0, Net::MessageBuffer);

		std::string aNewMessage = std::string(aMessage.begin() + aMinValue, aMessage.begin() + aMaxValue);
		ChatMessage aChatMessage;
		aChatMessage.SetId(aNetMessage->GetId());
		aChatMessage.SetSenderId(aSenderId);
		aChatMessage.SetTargetId(aTargetId);
		aChatMessage.SetColor(aNetMessage->GetColor());
		aChatMessage.SetSender(aNetMessage->GetSender());
		aChatMessage.SetMessage(aNewMessage);

		memcpy_s(buffer, Net::MessageBuffer, &aChatMessage, Net::MessageBuffer);
		const auto result = sendto(mySocket, buffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myClientAddress[aTargetId]), sizeof(myClientAddress[aTargetId]));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}

		//opposite of inet_pton, converts an ip adress from binary format to a readable format.
		char address[16]{ 0 };
		inet_ntop(AF_INET, &myClientAddress[aTargetId].sin_addr, &address[0], 16);
		const auto fromPort = ntohs(myClientAddress[aTargetId].sin_port);
		//Change to UserName
		std::cout << aChatMessage.GetBuffer() << std::endl;
		//std::cout << "Packet received from " << address << " : " << fromPort << std::endl;

		aMessageCount -= aSize;

		aMinValue += aSize;
	} while (aMessageCount > 0);
}

void Server::SendGameMessage(GameMessage* aGameMessage, const short aTargetId, const short aSenderId, bool aGuaranteed)
{
	char buffer[Net::MessageBuffer];
	memset(buffer, 0, Net::MessageBuffer);
	if (myClientAddress.find(aTargetId) != myClientAddress.end())
	{
		if (aGuaranteed)
		{
			if (myGuaranteedGameMessages.find(aGameMessage->GetGuaranteedId()) == myGuaranteedGameMessages.end())
			{
				myTotalPacketsList[aGameMessage->GetTargetId()]++;
				aGameMessage->SetGuaranteedId(++myGuaranteedCounter);
				aGameMessage->SetTargetId(aTargetId);
				myGuaranteedGameMessages.insert({ aGameMessage->GetGuaranteedId(), aGameMessage });
				myGuaranteedMessagesTimers.insert({ aGameMessage->GetGuaranteedId(), 0.0f });
			//	std::cout << "Sent guaranteed message id: " << std::to_string(aGameMessage->GetGuaranteedId()).c_str() << std::endl;
			}
		}

		memcpy_s(buffer, Net::MessageBuffer, aGameMessage, Net::MessageBuffer);
		const auto result = sendto(mySocket, buffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myClientAddress[aTargetId]), sizeof(myClientAddress[aTargetId]));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}

		//opposite of inet_pton, converts an ip adress from binary format to a readable format.
		char address[16]{ 0 };
		inet_ntop(AF_INET, &myClientAddress[aTargetId].sin_addr, &address[0], 16);
		const auto fromPort = ntohs(myClientAddress[aTargetId].sin_port);
		//Change to UserName
	}
}

void Server::ProcessMessage(char* aBuffer, sockaddr_in aFromAddress)
{
	MessageType aMesssageType = static_cast<MessageType>(aBuffer[0]);

	NetMessage* packetCheckMessage = reinterpret_cast<NetMessage*>(aBuffer);

	if (myClientAddress.find(packetCheckMessage->GetSenderId()) != myClientAddress.end()) 
	{
		float packetLoss;

		if(myLostPacketsList[packetCheckMessage->GetSenderId()] == 0)
		{
			packetLoss = 0;
		}
		else 
		{
			packetLoss = (myLostPacketsList[packetCheckMessage->GetSenderId()] / myTotalPacketsList[packetCheckMessage->GetSenderId()]) * 10;
		}
		const auto fromPort = ntohs(myClientAddress[packetCheckMessage->GetSenderId()].sin_port);
		char address[16]{ 0 };
		inet_ntop(AF_INET, &myClientAddress[packetCheckMessage->GetSenderId()].sin_addr, &address[0], 16);
		std::cout << "Packet received from " << address << " : " << fromPort << " : " << std::to_string(packetLoss) << "% Packet loss so far" << std::endl;
	}

	switch (aMesssageType)
	{
	case MessageType::none:
		break;
	case MessageType::ConnectMessage:
	{
		ConnectMessage* aConnectMessage = reinterpret_cast<ConnectMessage*>(aBuffer);
		myIdCounter++;
		myClientAddress.insert({ myIdCounter , aFromAddress });
		myClientNames.insert({ myIdCounter, aConnectMessage->GetBuffer() });
		myTimeOutTimers.insert({ myIdCounter, myPingTimeOut });
		std::string connectMessage = aConnectMessage->GetBuffer();
		connectMessage += " joined.";
		char aNumber;
		aNumber = (char)myIdCounter;
		const auto result = sendto(mySocket, &aNumber, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&aFromAddress), sizeof(aFromAddress));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}

		for (int i = 10; i > 0; i--)
		{
			if (myMessageHistory.size() > 0) 
			{
				if (i > myMessageHistory.size())
				{
					i = myMessageHistory.size();
				}
				SendClientMessage(&myMessageHistory[myMessageHistory.size() - i], myIdCounter, 0);
			}
		}

		SendServerMessage(connectMessage);
		break;
	}
	case MessageType::DisconnectMessage:
	{
		DisconnectMessage* aConnectMessage = reinterpret_cast<DisconnectMessage*>(aBuffer);
		std::string disconnectMessage = myClientNames.at(aConnectMessage->GetSenderId());
		disconnectMessage += " left.";
		char aNumber;
		aNumber = (char)aConnectMessage->GetSenderId();
		const auto result = sendto(mySocket, &aNumber, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&aFromAddress), sizeof(aFromAddress));
		if (result == SOCKET_ERROR)
		{
			PrintLastError();
		}

		DisconnectUser(aConnectMessage->GetSenderId());
		SendServerMessage(disconnectMessage);
		break;
	}
	case MessageType::ChatMessage:
	{
		ChatMessage* aChatMessage = reinterpret_cast<ChatMessage*>(aBuffer);
		myTimeOutTimers[aChatMessage->GetSenderId()] = myPingTimeOut;
		myMessageHistory.push_back(*aChatMessage);
		for (auto& clients : myClientAddress)
		{
			SendClientMessage(aChatMessage, clients.first, aChatMessage->GetSenderId());
		}
		break;
	}
	case MessageType::PingMessage: 
	{
		PingMessage* aChatMessage = reinterpret_cast<PingMessage*>(aBuffer);
		std::cout << "Recived ping from " << myClientNames[aChatMessage->GetSenderId()] << " it took " << -(myTimeOutTimers[aChatMessage->GetSenderId()] + 1) << " second/s." << std::endl;
		myTimeOutTimers[aChatMessage->GetSenderId()] = myPingTimeOut;
		break;
	}
	case MessageType::GameMessage:
	{
		GameMessage* aGameCommand = reinterpret_cast<GameMessage*>(aBuffer);
		myTimeOutTimers[aGameCommand->GetSenderId()] = myPingTimeOut;
		switch ((GameCommand)aGameCommand->GetCommand())
		{
			case GameCommand::CreateRoom:
			{
				NetCreateRoom* aRoomMessage = reinterpret_cast<NetCreateRoom*>(aBuffer);
				bool roomExists = false;
				for (auto& room : myRoomNames)
				{
					if (room.second == aRoomMessage->GetRoomName()) 
					{
						myRooms[room.first].JoinRoom(aRoomMessage->GetSenderId());
						std::cout << myClientNames[aRoomMessage->GetSenderId()].c_str() << " Joined Room: " << room.second << std::endl;
						roomExists = true;
						break;
					}
				}
				if (!roomExists) 
				{
					auto aId = CreateRoom(aRoomMessage->GetRoomName(), aRoomMessage->GetGameName());
					std::cout << myClientNames[aRoomMessage->GetSenderId()].c_str() << " Created Room: " << myRoomNames[aId] << std::endl;
					myRooms[aId].JoinRoom(aRoomMessage->GetSenderId());
					std::cout << myClientNames[aRoomMessage->GetSenderId()].c_str() << " Joined Room: " << myRoomNames[aId] << std::endl;
					break;
				}
			}
			case GameCommand::JoinRoom:
			{
				NetJoinRoom* aRoomMessage = reinterpret_cast<NetJoinRoom*>(aBuffer);
				for (auto room : myRoomNames)
				{
					if (room.second == aRoomMessage->GetRoomName()) 
					{
						myRooms[room.first].JoinRoom(aRoomMessage->GetSenderId());
						std::cout << myClientNames[aRoomMessage->GetSenderId()].c_str() << " Joined Room: " << room.second << std::endl;
						break;
					}
				}
				break;
			}
			default:
			{
				for (auto room : myRoomNames)
				{
					if (myRooms[room.first].GetGame()->Running()) 
					{
						myRooms[room.first].GetGame()->ProcessMessage(aGameCommand);
					}
				}
				break;
			}

		}
		break;
	}
	case MessageType::RecivedGuaranteedMessage: 
	{
		NetMessage* recivedMessage = reinterpret_cast<NetMessage*>(aBuffer);
		myTimeOutTimers[recivedMessage->GetSenderId()] = myPingTimeOut;

		std::cout << "Recived guarenteed id: " << std::to_string(recivedMessage->GetGuaranteedId()).c_str() << std::endl;
		//mytempDeletedthingys.push_back(recivedMessage->GetGuaranteedId());
		
		if (myGuaranteedGameMessages.find(recivedMessage->GetGuaranteedId()) != myGuaranteedGameMessages.end()) 
		{
			delete myGuaranteedGameMessages[recivedMessage->GetGuaranteedId()];
			myGuaranteedGameMessages[recivedMessage->GetGuaranteedId()] = nullptr;
		}
		else 
		{
			myLostPacketsList[recivedMessage->GetTargetId()]--;
			std::cout << "Tried to remove message that did not exists" << std::endl;
		}
		break;
	}
	}
}

void Server::PingAllUsers()
{
}

void Server::PingUser(const short& aId)
{
	PingMessage* aPing = new PingMessage();
	aPing->SetId(0);
	aPing->SetTargetId(aId);
	aPing->SetSenderId(0);

	char buffer[Net::MessageBuffer];
	memset(buffer, 0, Net::MessageBuffer);
	memcpy_s(buffer, Net::MessageBuffer, aPing, Net::MessageBuffer);

	const auto result = sendto(mySocket, buffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&myClientAddress[aId]), sizeof(myClientAddress[aId]));

	std::cout << "Pinged " << myClientNames[aId] << std::endl;

	delete aPing;
}

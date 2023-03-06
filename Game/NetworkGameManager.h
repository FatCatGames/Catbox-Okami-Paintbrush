#pragma once
#include "GameObjects/Component.h"
#include "..\CatNet\NetworkGameCommands.h"
#include "Networking/Client.h"
#include "NetworkComponent.h"
class NetworkGameManager
{
public:
	NetworkGameManager();
	~NetworkGameManager();
	void Update();
	void HandleMessage(NetMessagePayload* aPayload);
	void AddListener(ListenerWithParameterAndID& aListener, GameCommand aCommand);
	void RemoveListener(ListenerWithParameterAndID& aListener, GameCommand aCommand);
	void RequestID(NetworkComponent* aComponentCallback);
	void JoinGame();
	static NetworkGameManager* GetInstance();
private:
	static NetworkGameManager* Instance;
	UINT myId = 0;
	ListenerWithParameter myListener;
	std::vector<NetworkComponent*> myCallbackComponents;
	std::array<Event, (int)GameCommand::Count> myEvent;
};


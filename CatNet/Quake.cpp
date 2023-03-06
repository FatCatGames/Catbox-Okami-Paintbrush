#include "Quake.h"

void Quake::Start(std::vector<unsigned short> aPlayerList)
{
	NetStartGame* startGameMessage = new NetStartGame();
	startGameMessage->SetSceneName("Quake");
	startGameMessage->SetSenderId(0);
	startGameMessage->SetNetworkComponentID(myObjectIdCounter);
	AddMessage(startGameMessage);
	myPlayerIDs = aPlayerList;
	//for (int i = 0; i < aPlayerList.size(); i++)
	//{
	//	NetSpawnPrefabCommand* spawnMessage = new NetSpawnPrefabCommand();
	//	spawnMessage->SetNetworkComponentID(++myObjectIdCounter);
	//	spawnMessage->SetSpawnPosition(mySpawnPositions[i]);
	//	spawnMessage->SetSenderId(aPlayerList[i]);
	//	spawnMessage->SetTargetId(aPlayerList[i]);
	//	spawnMessage->SetPrefabName("Client_Dummy_prefab");
	//	AddMessage(spawnMessage);
	//}
	myRunning = true;
}

void Quake::Update()
{
	myTimer.Update();
	if (myStartTimer < -1)
	{
		myCurrentCooldown -= myTimer.GetDeltaTime();
		myStepTimer -= myTimer.GetDeltaTime();
		std::vector<unsigned short> removedIds;
		if (myStepTimer <= 0)
		{
			myStepTimer = 1.0f / 30.0f;

			if (myCurrentCooldown <= 0 && myObjects.size() < 10)
			{
				object aObject;
				NetSpawnPrefabCommand* spawnMessage = new NetSpawnPrefabCommand();
				spawnMessage->SetNetworkComponentID(++myObjectIdCounter);
				spawnMessage->SetSpawnPosition((mySpawnPositions[0] + mySpawnPositions[1]) * 0.5f);
				spawnMessage->SetSenderId(0);
				spawnMessage->SetPrefabName("Network Sphere");
				spawnMessage->SetGuaranteed(true);
				AddMessage(spawnMessage);
				myStartTimer = -2.0f;
				aObject.pos = spawnMessage->GetSpawnPosition();
				myObjects.insert({ myObjectIdCounter, aObject });
				myCurrentCooldown = mySpawnCooldown;
			}

			for (auto& object : myObjects)
			{
				NetMoveCommand* moveObject = new NetMoveCommand();
				moveObject->SetNetworkComponentID(object.first);
				if (object.second.pos.x > 10 || object.second.pos.x < -10)
				{
					object.second.dir = object.second.dir * -1.0f;
				}
				moveObject->SetPosition(object.second.pos + ((object.second.dir * 7.0f) * myStepTimer));
				moveObject->SetSenderId(0);
				object.second.pos = moveObject->GetPosition();
				AddMessage(moveObject);
			}
			for (auto& object : myObjects)
			{
				bool exisits = false;
				for (size_t i = 0; i < removedIds.size(); i++)
				{
					if (object.first == removedIds[i])
					{
						exisits = true;
						break;
					}
				}
				if (!exisits) 
				{
					for (auto& otherObject : myObjects)
					{
						if (otherObject.first != object.first)
						{
							bool exisits = false;
							for (size_t i = 0; i < removedIds.size(); i++)
							{
								if (otherObject.first == removedIds[i])
								{
									exisits = true;
									break;
								}
							}
							if (!exisits)
							{
								if (otherObject.second.pos.Distance(object.second.pos) < 0.5f)
								{
									NetDestroyObject* destroyOtherObject = new NetDestroyObject();
									destroyOtherObject->SetNetworkComponentID(otherObject.first);
									destroyOtherObject->SetSenderId(0);
									destroyOtherObject->SetGuaranteed(true);
									AddMessage(destroyOtherObject);
									NetDestroyObject* destroyObject = new NetDestroyObject();
									destroyObject->SetNetworkComponentID(object.first);
									destroyObject->SetSenderId(0);
									destroyObject->SetGuaranteed(true);
									AddMessage(destroyObject);
									removedIds.push_back(object.first);
									removedIds.push_back(otherObject.first);
								}
							}
						}
					}
				}
			}
			for (int i = 0; i < removedIds.size(); i++)
			{
				myObjects.erase(removedIds[i]);
			}
		}
	}
	else
	{
		myStartTimer -= myTimer.GetDeltaTime();
		if (myStartTimer <= 0)
		{
			for (int i = 0; i < myPlayerIDs.size(); i++)
			{
				NetSpawnPrefabCommand* spawnMessage = new NetSpawnPrefabCommand();
				spawnMessage->SetNetworkComponentID(++myObjectIdCounter);
				spawnMessage->SetSpawnPosition(mySpawnPositions[i]);
				spawnMessage->SetSenderId(myPlayerIDs[i]);
				spawnMessage->SetTargetId(myPlayerIDs[i]);
				spawnMessage->SetPrefabName("Client_Dummy_prefab");
				AddMessage(spawnMessage);
				myStartTimer = -2.0f;
			}
		}
	}
}

void Quake::ProcessMessage(GameMessage* aGameMessage)
{
	switch ((GameCommand)aGameMessage->GetCommand())
	{
	case GameCommand::MoveCommand:
	{
		NetMoveCommand* aMoveCommand = reinterpret_cast<NetMoveCommand*>(aGameMessage);

		for (int i = 0; i < myPlayerIDs.size(); i++)
		{
			NetMoveCommand sendMoveCommand = *aMoveCommand;
			sendMoveCommand.SetTargetId(myPlayerIDs[i]);
			AddMessage(new NetMoveCommand(sendMoveCommand));
		}
		break;
	}
	case GameCommand::RotateCommand:
	{
		NetRotateCommand* aRotateCommand = reinterpret_cast<NetRotateCommand*>(aGameMessage);

		for (int i = 0; i < myPlayerIDs.size(); i++)
		{
			NetRotateCommand sendRotateCommand = *aRotateCommand;
			sendRotateCommand.SetTargetId(myPlayerIDs[i]);
			AddMessage(new NetRotateCommand(sendRotateCommand));
		}
		break;
	}
	case GameCommand::FireCommand:
	{

		break;
	}
	case GameCommand::InteractCommand:
	{

		break;
	}
	case GameCommand::SpawnPrefabCommand:
	{
		if (aGameMessage->GetNetworkComponentID() == 0)
		{
			NetSpawnPrefabCommand* aSpawnCommand = reinterpret_cast<NetSpawnPrefabCommand*>(aGameMessage);
			aSpawnCommand->SetNetworkComponentID(++myObjectIdCounter);
			for (int i = 0; i < myPlayerIDs.size(); i++)
			{
				NetSpawnPrefabCommand sendSpawnCommand = *aSpawnCommand;
				sendSpawnCommand.SetTargetId(myPlayerIDs[i]);
				AddMessage(new NetSpawnPrefabCommand(sendSpawnCommand));
			}
		}
		break;
	}
	case GameCommand::SpawnControllableObject:
	{

		break;
	}
	case GameCommand::CreateRoom:
	{

		break;
	}
	}
}
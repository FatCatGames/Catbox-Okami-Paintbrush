#include "Game.pch.h"
#include "NetworkGameManager.h"
#include "Networking/NetworkManager.h"
#include "..\CatNet\NetworkGameCommands.h"
#include "Managers/NetMessageHandler.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "ComponentTools/SceneManager.h"

NetworkGameManager* NetworkGameManager::Instance;

NetworkGameManager::~NetworkGameManager()
{
}

NetworkGameManager::NetworkGameManager()
{
	if (Instance) return;
	auto networkManager = Engine::GetInstance()->GetNetworkManager();
	Instance = this;
	myId = networkManager->GetNextId();
	myListener.action = [this](void* data) {HandleMessage(reinterpret_cast<NetMessagePayload*>(data)); };
	networkManager->RegisterMessageListener(myListener);

	std::string ip = Editor::GetInstance() ? Editor::GetInstance()->GetUserPrefs().myDefaultIP : "127.0.0.1";
	std::string username = Editor::GetInstance() ? Editor::GetInstance()->GetUserPrefs().myUsername : "User";
	Engine::GetInstance()->GetNetworkManager()->Connect(username, ip, 42000);


	/*NetSpawnPrefabCommand* aSpawnMessage = new NetSpawnPrefabCommand();
	NetMessagePayload aPayload;
	aSpawnMessage->SetNetworkComponentID(0);
	aSpawnMessage->SetPrefabName("Client_Dummy_prefab");
	aSpawnMessage->SetSpawnPosition({ 0, -2.0f, 0 });
	aPayload.message = aSpawnMessage;
	aPayload.messageType = MessageType::GameMessage;
	Engine::GetInstance()->GetNetworkManager()->SendNetMessage(aPayload);*/
}

void NetworkGameManager::Update()
{
	//if (Input::GetKeyPress(KeyCode::SPACE)) 
	//{
	//	NetSpawnPrefabCommand* aSpawnCommand = new NetSpawnPrefabCommand();
	//	aSpawnCommand->SetPrefabName("Grenade Ammo Container");
	//	aSpawnCommand->SetSpawnPosition({ 0, 0, 0 });
	//	aSpawnCommand->SetSpawnRotation({ 0,0,0,0 });
	//	NetMessagePayload aPayLoad;
	//	aPayLoad.message = aSpawnCommand;
	//	aPayLoad.messageType = MessageType::GameMessage;
	//	NetMessageHandler::GetInstance()->SendNetMessage(aPayLoad);
	//}
}

void NetworkGameManager::HandleMessage(NetMessagePayload* aPayload)
{
	if (aPayload->messageType == MessageType::GameMessage)
	{
		GameMessage* aGameMessage = reinterpret_cast<GameMessage*>(aPayload->message);

		switch ((GameCommand)aGameMessage->GetCommand())
		{
		case GameCommand::MoveCommand:
		{
			NetMoveCommand* aMoveCommand = reinterpret_cast<NetMoveCommand*>(aPayload->message);
			myEvent[(int)GameCommand::MoveCommand].Invoke((void*)&aMoveCommand->GetPosition(), aMoveCommand->GetNetworkComponentID());
			break;
		}
		case GameCommand::RotateCommand:
		{
			NetRotateCommand* aRotateCommand = reinterpret_cast<NetRotateCommand*>(aPayload->message);
			myEvent[(int)GameCommand::RotateCommand].Invoke((void*)&aRotateCommand->GetQuaternion(), aRotateCommand->GetNetworkComponentID());
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
			NetSpawnPrefabCommand* aSpawnCommand = reinterpret_cast<NetSpawnPrefabCommand*>(aGameMessage);
			std::string tempName = (aSpawnCommand->GetPrefabName());
			GameObject* aGameObject = InstantiatePrefab(aSpawnCommand->GetPrefabName());
			if (aGameObject)
			{
				auto networkComponent = aGameObject->GetComponent<NetworkComponent>();
				if (networkComponent)
				{
					aGameObject->GetTransform()->SetWorldPos(aSpawnCommand->GetSpawnPosition());
					networkComponent->SetGameNetworkID(aSpawnCommand->GetNetworkComponentID());
					if (aGameMessage->GetSenderId() == Engine::GetInstance()->GetNetworkManager()->GetClientId())
					{
						networkComponent->ClientAwake();

						//temp
						if (PLAYMODE)
						{
							Engine::GetInstance()->SetActiveCamera(aGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<Camera>());
						}
					}
					else
					{
						//temp
						if (aGameMessage->GetSenderId() != 0) 
						{
							aGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->RemoveComponent(aGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<Camera>());
						}
						networkComponent->DummyAwake();
					}

					//temp
					if (aGameMessage->GetNetworkComponentID() == 1)
					{
						aGameObject->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Player_1_mat"), 0);
					}
					//else if (aGameMessage->GetNetworkComponentID() == 2)
					//{
					//	aGameObject->GetTransform()->GetChildren()[1]->GetGameObject()->GetComponent<ModelInstance>()->SetMaterial(AssetRegistry::GetInstance()->GetAsset<Material>("Player_2_mat"), 0);
					//}
				}
			}
			else
			{
				printerror("Could not find gameobject!");
			}
			break;
		}
		case GameCommand::SpawnControllableObject:
			break;

		case GameCommand::IdRequest:
			break;
		case GameCommand::StartGame:
		{
			NetStartGame* aStartGameCommand = reinterpret_cast<NetStartGame*>(aPayload->message);
			Engine::GetInstance()->GetSceneManager()->LoadScene(AssetRegistry::GetInstance()->GetAsset<Scene>("Quake"));
			break;
		}
		case GameCommand::DestroyObject: 
		{
			NetDestroyObject* aDestroyCommand = reinterpret_cast<NetDestroyObject*>(aPayload->message);
			myEvent[(int)GameCommand::DestroyObject].Invoke(nullptr, aDestroyCommand->GetNetworkComponentID());
			break;
		}
		default:
			break;
		} 
	}
}

void NetworkGameManager::AddListener(ListenerWithParameterAndID & aListener, GameCommand aCommand)
{
	myEvent[(int)aCommand].AddListener(aListener);
}

void NetworkGameManager::RemoveListener(ListenerWithParameterAndID& aListener, GameCommand aCommand)
{
	myEvent[(int)aCommand].RemoveListener(aListener);
}

void NetworkGameManager::RequestID(NetworkComponent* aComponentCallback)
{
	NetMessagePayload aPayLoad;
	GameMessage* idRequest = new GameMessage();
	idRequest->SetNetworkComponentID(0);
	idRequest->SetCommand((uint8_t)GameCommand::IdRequest);
	aPayLoad.message = idRequest;
	aPayLoad.messageType = MessageType::GameMessage;
	NetMessageHandler::GetInstance()->SendNetMessage(aPayLoad);
}

void NetworkGameManager::JoinGame()
{
	//NetCreateRoom* aJoinMessage = new NetCreateRoom();
	//NetMessagePayload aPayload;
	//aJoinMessage->SetRoomName("Temp");
	//aJoinMessage->SetGameName("Quake");
	//aJoinMessage->SetSlots(2);
	//aJoinMessage->SetTargetId(0);
	//aJoinMessage->SetNetworkComponentID(0);
	//
	//aPayload.message = aJoinMessage;
	//aPayload.messageType = MessageType::GameMessage;
	//Engine::GetInstance()->GetNetworkManager()->SendNetMessage(aPayload);
}


NetworkGameManager* NetworkGameManager::GetInstance()
{
	if (!Instance)
	{
		Instance = &NetworkGameManager();
	}
	return Instance;;
}



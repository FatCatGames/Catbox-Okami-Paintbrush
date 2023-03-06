#include "Game.pch.h"
#include "NetworkComponent.h"
#include "NetworkGameManager.h"
#include "Networking/NetworkManager.h"

void NetworkComponent::DummyAwake()
{
	myListeners[(int)GameCommand::MoveCommand].action = [this](void* aPos, unsigned short aID) { if (myGameNetworkID == aID) { Move(*reinterpret_cast<Vector3f*>(aPos)); } };
	myListeners[(int)GameCommand::RotateCommand].action = [this](void* aRot, unsigned short aID) {if (myGameNetworkID == aID) { Rotate(*reinterpret_cast<Vector4f*>(aRot)); } };
	myListeners[(int)GameCommand::FireCommand].action = [this](void* aValue, unsigned short aID) { if (myGameNetworkID == aID) { print("Fire"); } };
	myListeners[(int)GameCommand::InteractCommand].action = [this](void* aValue, unsigned short aID) {if (myGameNetworkID == aID) { print("Interact"); } };
	myListeners[(int)GameCommand::DestroyObject].action = [this](void* aValue, unsigned short aID) {if (myGameNetworkID == aID) { myGameObject->Destroy(); }};

	for (int i = 0; i < (int)GameCommand::Count; i++)
	{
		if (!myListeners[i].action)
		{
			myListeners[i].action = [this](void* aValue, unsigned short aID) { if (myGameNetworkID == aID) { print("Game Command"); }};
		}
	}
	for (int i = 0; i < (int)GameCommand::Count; i++)
	{
		NetworkGameManager::GetInstance()->AddListener(myListeners[i], static_cast<GameCommand>(i));
	}
	myClient = false;
}

void NetworkComponent::ClientAwake()
{
	//NetSpawnPrefabCommand* spawnMe = new NetSpawnPrefabCommand();
	myClient = true;
	//spawnMe->SetPrefabName("Client Dummy");
	//spawnMe->SetSpawnPosition(myTransform->worldPos());
	//spawnMe->SetSpawnRotation(Catbox::ToQuaternion(myTransform->worldRot()));
	//spawnMe->SetNetworkComponentID(0);
	//spawnMe->SetObjectId(myGameObject->GetObjectInstanceID());

	//NetMessagePayload aPayload;
	//aPayload.message = spawnMe;
	//aPayload.messageType = MessageType::GameMessage;
	//Engine::GetInstance()->GetNetworkManager()->SendNetMessage(aPayload);
}

void NetworkComponent::BindFunctions(std::function<void* (void*, unsigned short)> aLambda, GameCommand aCommand)
{
	NetworkGameManager::GetInstance()->RemoveListener(myListeners[(int)aCommand], aCommand);
	myListeners[(int)aCommand].action = aLambda;
	NetworkGameManager::GetInstance()->AddListener(myListeners[(int)aCommand], aCommand);
}

void NetworkComponent::SendNetMessage(GameMessage* aMessage)
{
	NetMessagePayload aPayload;
	aMessage->SetNetworkComponentID(myGameNetworkID);
	aPayload.message = aMessage;
	aPayload.messageType = MessageType::GameMessage;
	Engine::GetInstance()->GetNetworkManager()->SendNetMessage(aPayload);
}

void NetworkComponent::Update()
{
	if (myClient) 
	{
		Vector3f aDir = { 0,0,0 };
		bool rotated = false;
		if (Input::GetKeyHeld(KeyCode::A))
		{
			myTransform->Rotate(0, -90 * deltaTime, 0);
			rotated = true;
		}
		else if (Input::GetKeyHeld(KeyCode::D))
		{
			myTransform->Rotate(0, 90 * deltaTime, 0);
			rotated = true;
		}
		if (Input::GetKeyHeld(KeyCode::W))
		{
			aDir = myTransform->forward();
		}
		else if (Input::GetKeyHeld(KeyCode::S))
		{
			aDir = myTransform->back();
		}
		myTransform->SetWorldPos(myTransform->worldPos() + (aDir * 5.0f) * deltaTime);
		if (aDir.x != 0 || aDir.z != 0) 
		{
			NetMoveCommand* aMoveCommand = new NetMoveCommand();
			aMoveCommand->SetPosition(myTransform->worldPos());
			SendNetMessage(aMoveCommand);
		}
		if (rotated) 
		{
			NetRotateCommand* aRotateCommand = new NetRotateCommand();
			aRotateCommand->SetRotation(Catbox::ToQuaternion(myTransform->worldRot()).GetVector4f());
			SendNetMessage(aRotateCommand);
		}
	}
	else 
	{
		if (myIsMoving) 
		{
			myTransform->SetWorldPos(Catbox::Lerp(myTransform->worldPos(), myTargetPos, 7.0f * deltaTime));
			if (myTransform->worldPos().Distance(myTargetPos) < 0.0000001f)
			{
				myIsMoving = false;
			}
		}
	}
}

void NetworkComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::Checkbox("Client", &myClient);
	ImGui::Text(("Network ID: " + std::to_string(myGameNetworkID)).c_str());
}
void NetworkComponent::Move(Vector3f aPos)
{
	myIsMoving = true;
	myTargetPos = aPos;
}

void NetworkComponent::Rotate(Vector4f aRot)
{
	myTransform->SetWorldRot(Catbox::ToEulerAngles(Catbox::Quaternion(aRot)));
}

void NetworkComponent::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Bool>("Client", myClient);
}
void NetworkComponent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Client"))
	{
		myClient = aComponentData["Client"].GetBool();
	}
}

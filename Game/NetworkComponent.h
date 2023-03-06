#pragma once
#include "GameObjects\Component.h"
#include "..\Catbox\CommonUtilities\UtilityFunctions.hpp"
#include "..\CatNet\NetworkGameCommands.h"
#include "ComponentTools/Event.h"
class NetworkComponent : public Component
{
public:
	void DummyAwake();
	void ClientAwake();
	void BindFunctions(std::function<void*(void*, unsigned short)> aLambda, GameCommand aCommand);
	void SendNetMessage(GameMessage* aMessage);
	void SetGameNetworkID(unsigned short aId) { myGameNetworkID = aId; }
	unsigned short GetGameNetworkID() { return myGameNetworkID; }
	void Update();
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void Move(Vector3f aPos);
	void Rotate(Vector4f aRot);
	void Save(rapidjson::Value& aComponentData);
	void Load(rapidjson::Value& aComponentData);

private:
	unsigned short myGameNetworkID = -1;
	std::array<ListenerWithParameterAndID, (int)GameCommand::Count> myListeners;
	bool myClient = false;
	bool myIsMoving = false;
	Vector3f myTargetPos = Vector3f();
};


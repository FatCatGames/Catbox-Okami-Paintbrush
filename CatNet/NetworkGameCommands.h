#pragma once
#include "..\CatNet\NetMessage.h"
#include "..\Catbox\CommonUtilities\Vector.h"

enum class GameCommand : uint8_t
{
	MoveCommand,
	RotateCommand,
	FireCommand,
	InteractCommand,
	SpawnPrefabCommand,
	SpawnControllableObject,
	IdRequest,
	CreateRoom,
	JoinRoom,
	StartGame,
	QuitGame,
	DestroyObject,
	Count
};

class NetMoveCommand : public GameMessage
{
private:
	Vector3f myPosition;
public:
	NetMoveCommand() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::MoveCommand); mySize = sizeof(NetMoveCommand); }
	Vector3f GetPosition() { return myPosition; }
	void SetPosition(Vector3f aPos) { myPosition = aPos; }
};

class NetRotateCommand : public GameMessage
{
private:
	Vector4f myQuaternion;
public:
	NetRotateCommand() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::RotateCommand); mySize = sizeof(NetRotateCommand);}
	Vector4f GetQuaternion() { return myQuaternion; }

	void SetRotation(Vector4f aQuat)
	{
		myQuaternion = aQuat;
	}
};

class NetSpawnPrefabCommand : public GameMessage 
{
private:
	char myPrefabName[100];
	Vector3f mySpawnPosition;
	Vector4f mySpawnRotation;
	unsigned int myObjectID = 0;
public:
	NetSpawnPrefabCommand() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::SpawnPrefabCommand); mySize = sizeof(NetSpawnPrefabCommand);}

	std::string GetPrefabName() { return std::string(myPrefabName); }
	Vector3f GetSpawnPosition() { return mySpawnPosition; }
	Vector4f GetSpawnRotation() { return mySpawnRotation; }
	unsigned int GetObjectID() { return myObjectID; }

	void SetPrefabName(std::string aPrefabName) 
	{
		memset(myPrefabName, 0, 100);
		memcpy_s(myPrefabName, 100, aPrefabName.c_str(), aPrefabName.length());
	}
	void SetSpawnPosition(Vector3f aPos) 
	{
		mySpawnPosition = aPos;
	}
	void SetSpawnRotation(Vector4f aRotation)
	{
		mySpawnRotation = aRotation;
	}
	void SetObjectId(unsigned int aObjectId)
	{
		myObjectID = aObjectId;
	}
};

class NetSpawnControllableObject : public GameMessage
{
private:
	char myPrefabName[100];
	Vector3f mySpawnPosition;
	Vector4f mySpawnRotation;
public:
	NetSpawnControllableObject() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::SpawnControllableObject); mySize = sizeof(NetSpawnControllableObject); }

	std::string GetPrefabName() { return std::string(myPrefabName); }
	Vector3f GetSpawnPosition() { return mySpawnPosition; }
	Vector4f GetSpawnRotation() { return mySpawnRotation; }

	void SetPrefabName(std::string aPrefabName)
	{
		memset(myPrefabName, 0, 100);
		memcpy_s(myPrefabName, 100, aPrefabName.c_str(), aPrefabName.length());
	}
	void SetSpawnPosition(Vector3f aPos)
	{
		mySpawnPosition = aPos;
	}
	void SetSpawnRotation(Vector4f aRotation)
	{
		mySpawnRotation = aRotation;
	}
};

class NetCreateRoom : public GameMessage 
{
private:
	char myRoomName[100];
	char myGameName[100];
	unsigned int mySlots = 1;
public:
	NetCreateRoom() { myType = MessageType::GameMessage, myCommand = static_cast<uint8_t>(GameCommand::CreateRoom); mySize = sizeof(NetCreateRoom);}

	std::string GetRoomName() { return std::string(myRoomName); }

	std::string GetGameName() { return std::string(myGameName); }

	unsigned int GetSlots() { return mySlots; }

	void SetRoomName(const std::string& aRoomName) 
	{
		memset(myRoomName, 0, 100);
		memcpy_s(myRoomName, 100, aRoomName.c_str(), aRoomName.length());
	}
	void SetGameName(const std::string& aGameName)
	{
		memset(myGameName, 0, 100);
		memcpy_s(myGameName, 100, aGameName.c_str(), aGameName.length());
	}

	void SetSlots(unsigned int aAmmountSlots) 
	{
		mySlots = aAmmountSlots;
	}
};
class NetJoinRoom : public GameMessage
{
private:
	char myRoomName[100];

public:
	NetJoinRoom() { myType = MessageType::GameMessage, myCommand = static_cast<uint8_t>(GameCommand::JoinRoom);  mySize = sizeof(NetJoinRoom);}

	std::string GetRoomName() { return std::string(myRoomName); }

	void SetRoomName(const std::string& aRoomName)
	{
		memset(myRoomName, 0, 100);
		memcpy_s(myRoomName, 100, aRoomName.c_str(), aRoomName.length());
	}
};

class NetStartGame : public GameMessage
{
private:
	char myScene[100];
public:
	NetStartGame() { myType = MessageType::GameMessage, myCommand = static_cast<uint8_t>(GameCommand::StartGame);  mySize = sizeof(NetStartGame);}

	std::string GetSceneName() { return std::string(myScene); }

	void SetSceneName(const std::string& aSceneName)
	{
		memset(myScene, 0, 100);
		memcpy_s(myScene, 100, aSceneName.c_str(), aSceneName.length());
	}
};

class NetDestroyObject : public GameMessage
{
private:
public:
	NetDestroyObject() { myType = MessageType::GameMessage, myCommand = static_cast<uint8_t>(GameCommand::DestroyObject);  mySize = sizeof(NetStartGame); }
};
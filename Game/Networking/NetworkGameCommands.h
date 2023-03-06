#pragma once
#include "..\CatNet\NetMessage.h"
#include "CommonUtilities/Vector.h"

enum class GameCommand : uint8_t
{
	MoveCommand,
	RotateCommand,
	FireCommand,
	InteractCommand,
	SpawnPrefabCommand,
	SpawnControllableObject,
	IdRequest,
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
	Catbox::Quaternion myQuaternion;
public:
	NetRotateCommand() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::RotateCommand); mySize = sizeof(NetRotateCommand);}
	Catbox::Quaternion GetQuaternion() { return myQuaternion; }

	void SetRotation(Catbox::Quaternion aQuat)
	{
		myQuaternion = aQuat;
	}
};

class NetSpawnPrefabCommand : public GameMessage 
{
private:
	char myPrefabName[100];
	Vector3f mySpawnPosition;
	Catbox::Quaternion mySpawnRotation;
	unsigned int myObjectID = 0;
public:
	NetSpawnPrefabCommand() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::SpawnPrefabCommand); mySize = sizeof(NetSpawnPrefabCommand) + 100; }

	std::string GetPrefabName() { return std::string(myPrefabName); }
	Vector3f GetSpawnPosition() { return mySpawnPosition; }
	Catbox::Quaternion GetSpawnRotation() { return mySpawnRotation; }
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
	void SetSpawnRotation(Catbox::Quaternion aRotation)
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
	NetSpawnControllableObject() { myType = MessageType::GameMessage; myCommand = static_cast<uint8_t>(GameCommand::SpawnControllableObject);mySize = sizeof(NetSpawnControllableObject); }

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
#pragma once
#include <string>
enum class MessageType : uint8_t
{
	none,
	ConnectMessage,
	DisconnectMessage,
	ChatMessage,
	ServerMessage,
	PingMessage,
	GameMessage, 
	RecivedGuaranteedMessage
};


class NetMessage
{
protected:
	MessageType myType = MessageType::none;
	unsigned short myId = -1;
	unsigned short mySenderId = -1;
	unsigned short myTargetId = -1;
	unsigned short myGuaranteedId = -1;
	bool myGuaranteed = false;
public:
	NetMessage() = default;
	NetMessage(short aId, short aSenderId, short aTargetId) : myId(aId), mySenderId(aSenderId), myTargetId(aTargetId) {};
	short GetId() { return myId; }
	short GetSenderId() { return mySenderId; }
	short GetTargetId() { return myTargetId; }
	short GetGuaranteedId() { return myGuaranteedId; }
	bool GetGuaranteed() { return myGuaranteed; }
	void SetId(const short& aId) { myId = aId; }
	void SetSenderId(const short& aId) { mySenderId = aId; }
	void SetTargetId(const short& aId) { myTargetId = aId; }
	void SetGuaranteedId(const short& aId) { myGuaranteedId = aId; }
	void SetGuaranteed(const bool aFlag) { myGuaranteed = aFlag; }
	MessageType GetMessageType() { return myType; }

private:

};

class PingMessage : public NetMessage
{
public:
	PingMessage() { myType = MessageType::PingMessage; }
};

class ChatMessage : public NetMessage
{
protected:
	char myMessage[471];
	char mySender[20];
	char myColor[6];
public:
	ChatMessage() { myType = MessageType::ChatMessage; memset(myMessage, 0, sizeof(myMessage)); memset(mySender, 0, sizeof(mySender)); }

	void SetMessage(const std::string& aMessage)
	{
		memset(myMessage, 0, sizeof(myMessage));
		memcpy_s(myMessage, GetMessageSize(), aMessage.c_str(), aMessage.length());
	}
	void SetSender(const std::string& aSender)
	{
		memset(mySender, 0, sizeof(mySender));

		if (aSender.size() <= sizeof(mySender))
		{
			memcpy_s(mySender, sizeof(aSender), aSender.c_str(), aSender.length());
		}
	}
	void SetColor(const std::string& aHEX)
	{
		memset(myColor, 0, sizeof(myColor));
		memcpy_s(myColor, sizeof(aHEX), aHEX.c_str(), aHEX.length());
	}
	char* GetBuffer()
	{
		return &myMessage[0];
	}
	char* GetColor()
	{
		return &myColor[0];
	}
	char* GetSender()
	{
		return &mySender[0];
	}
	static int GetMessageSize() { return 474; }
};

class ServerMessage : public ChatMessage
{
public:
	ServerMessage() { myType = MessageType::ServerMessage; memset(myMessage, 0, sizeof(myMessage)); }

};

class ConnectMessage : public NetMessage
{
public:
	ConnectMessage() { myType = MessageType::ConnectMessage; memset(myMessage, 0, sizeof(myMessage)); }

	void SetMessage(const std::string& aMessage)
	{
		memset(myMessage, 0, sizeof(myMessage));
		memcpy_s(myMessage, GetMessageSize(), aMessage.c_str(), aMessage.length());
	}
	char* GetBuffer()
	{
		return &myMessage[0];
	}
	static int GetMessageSize()
	{
		return 500;
	}
private:
	char myMessage[500];
};

class DisconnectMessage : public NetMessage
{
public:
	DisconnectMessage() { myType = MessageType::DisconnectMessage; memset(myMessage, 0, sizeof(myMessage)); }

	void SetMessage(const std::string& aMessage)
	{
		memset(myMessage, 0, sizeof(myMessage));
		memcpy_s(myMessage, GetMessageSize(), aMessage.c_str(), aMessage.length());
	}
	char* GetBuffer()
	{
		return &myMessage[0];
	}
	static int GetMessageSize()
	{
		return 500;
	}
private:
	char myMessage[500];
};

class GameMessage : public NetMessage
{
public:
	GameMessage() { myType = MessageType::GameMessage; myCommand = 0; mySize = sizeof(GameMessage); }
	uint8_t GetCommand() { return myCommand; }
	unsigned short GetNetworkComponentID() { return myNetworkComponentID; }
	size_t GetMessageSize() { return mySize; }
	void SetNetworkComponentID(unsigned short aID) { myNetworkComponentID = aID; }
	void SetCommand(uint8_t aCommand) { myCommand = aCommand; }
protected:
	uint8_t myCommand;
	unsigned short myNetworkComponentID = -1;
	unsigned short myRoomId = -1;
	size_t mySize = 0;
};


class GuarateedMessageValidate : public NetMessage
{
public:
	GuarateedMessageValidate() { myType = MessageType::RecivedGuaranteedMessage; mySize = sizeof(GuarateedMessageValidate); }
protected:
	size_t mySize = 0;
};

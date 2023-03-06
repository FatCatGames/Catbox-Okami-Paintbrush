#pragma once
#include "..\CatNet\NetMessage.h"


struct NetMessagePayload
{
	void DeleteMessage() 
	{
		if (message) 
		{
			delete message;
		}
	}
	NetMessage* message;
	MessageType messageType;
};
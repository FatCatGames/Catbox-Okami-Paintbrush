#pragma once
#include "ComponentTools\Event.h"
#include "Networking\NetMessagePayload.h"

namespace CatNet 
{
	class NetworkManager;
}
class NetMessageHandler
{
public:
	static inline NetMessageHandler* GetInstance() { return Instance; }
	NetMessageHandler();
	~NetMessageHandler() = default;
	void SendNetMessage(NetMessagePayload& aPayload);

private:
	static NetMessageHandler* Instance;
	ListenerWithParameter myMessageListener;
	void HandleMessage(NetMessagePayload* aPayload);
	std::shared_ptr<CatNet::NetworkManager> myNetworkManager;
};
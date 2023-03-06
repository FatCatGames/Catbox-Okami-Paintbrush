#pragma once
#include <iostream>
#include <WinSock2.h>
#include "ws2tcpip.h"
#include "NetMessage.h"
#include <unordered_map>
#include "Timer.h"
#include "NetworkGameCommands.h"
#include "Game.h"
#include <mutex>

namespace Net
{
    const size_t MessageBuffer = 512;
    const int DefaultPort = 42000;
}


class Server
{
public:
    Server();
    void PrintLastError();

    void UpdateTimer();
    void UpdateGame();

    void Listen();

    SOCKET& GetSocket()  { return mySocket; }
    WSADATA& GetSocketData()  { return mySocketData; }
    sockaddr_in& GetAddress()  { return myAddress; }
    void DisconnectAllUsers();

private:
    void SendClientMessage(ChatMessage* aNetMessage, const short aTargetId, const short aSenderId, bool aGuaranteed = false);
    void SendGameMessage(GameMessage* aBuffer, const short aTargetId, const short aSenderId, bool aGuaranteed = false);
    void SendServerMessage(const std::string& aMessage);
    void DisconnectUser(const short& aId);
    unsigned short CreateRoom(const std::string& aRoomName, const std::string& aGameName);
    //void SendClientMessage(NetMessage* aNetMessage, std::string aTargetName);

    void ProcessMessage(char* aBuffer, sockaddr_in aFromAddress);
    void PingAllUsers();
    void PingUser(const short& aId);

    std::unordered_map<short, std::string> myClientNames;
    std::unordered_map<short, sockaddr_in> myClientAddress;
    std::unordered_map<short, float> myTimeOutTimers;
    std::unordered_map<short, std::string> myRoomNames;
    std::unordered_map<short, Room> myRooms;
    std::unordered_map<short, float> myGuaranteedMessagesTimers;
    std::unordered_map<short, GameMessage*> myGuaranteedGameMessages;
    std::unordered_map<short, int> myLostPacketsList;
    std::unordered_map<short, int> myTotalPacketsList;
    
    std::vector<short> myGuaranteedRemovedIDs;
    std::vector<short> mytempDeletedthingys;
    std::vector<GameMessage*> myGameMessages;
    std::vector<ChatMessage> myMessageHistory;
    std::vector<NetSpawnPrefabCommand> myPlayerSpawnHistory;


    SOCKET mySocket;
    WSADATA mySocketData;
    char aBuffer[Net::MessageBuffer];
    char messageBuffer[Net::MessageBuffer]{ "Connect" };
    unsigned int myIdCounter = 0;
    unsigned int myObjectIdCounter = 0;
    unsigned int myRoomIdCounter = 0;
    unsigned short myGuaranteedCounter = 0;
    float myTimeOutMax = 20;
    float myPingTimeOut = 10;

    Catbox::Timer myTimer;

    sockaddr_in myAddress;
    std::mutex myGuarrentedLock;
    
};


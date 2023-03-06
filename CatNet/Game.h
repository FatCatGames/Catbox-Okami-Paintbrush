#pragma once
#include "NetMessage.h"
#include <vector>

class Game
{
public:
    virtual void Start(std::vector<unsigned short> aPlayerIDs) = 0;
    virtual void Update() = 0;
    virtual bool Running() { return myRunning; }
    virtual void Stop() { myRunning = false; }
    void ClearMessages();
    std::vector<GameMessage*>& GetMessages() { return myMessages; }
    void AddMessage(GameMessage* aGameMessage) { myMessages.push_back(aGameMessage); }
    virtual void ProcessMessage(GameMessage* aGameMessage) = 0;
    void ReceiveServerMessage(GameMessage* aGameMessage) { myServerMessages.push_back(aGameMessage); }
protected:
    bool myRunning = false;
    unsigned int myObjectIdCounter = 0;

    std::vector<unsigned short> myPlayerIDs;
    std::vector<GameMessage*> myMessages;
    std::vector<GameMessage*> myServerMessages;
};
struct Room
{
    //Room(Game* aGame, unsigned int aSlots, unsigned int aRoomID);
    Game* myGame;
    unsigned int mySlots;
    unsigned int myRoomID;
    std::vector<unsigned short> myPlayerIds;

    bool JoinRoom(unsigned short aId)
    {
        if (myPlayerIds.size() < mySlots && !myGame->Running())
        {
            myPlayerIds.push_back(aId);
            return true;
        }
        return false;
    }
    void SetRoomId(unsigned short aRoomID)
    {
        myRoomID = aRoomID;
    }
    void ResetRoom() 
    {
        myPlayerIds.clear();
        delete myGame;
    }

    void SetGame(Game* aGame) { myGame = aGame; }
    unsigned short GetId() { return myRoomID; }
    Game* GetGame() { return myGame; }
    bool IsFull() { return myPlayerIds.size() >= mySlots; }
};

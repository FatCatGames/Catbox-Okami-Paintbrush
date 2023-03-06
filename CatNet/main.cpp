#include <iostream>
#include "Server.h"


void PrintLastError()
{
    int error = WSAGetLastError();
    std::cout << "Error " << error << std::endl;
}

int main()
{
    Server aServer;

    aServer.GetAddress().sin_family = AF_INET;

    auto myAddress = aServer.GetAddress();
    auto  mySocket = aServer.GetSocket();
    auto mySocketData = aServer.GetSocketData();

    char messageBuffer[Net::MessageBuffer];
    //Client
    inet_pton(AF_INET, "127.0.0.1", &myAddress.sin_addr.s_addr);
    myAddress.sin_port = htons(Net::DefaultPort);

    //Server
    myAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(mySocket, reinterpret_cast<sockaddr*>(&myAddress), sizeof(myAddress)) == SOCKET_ERROR)
    {
        PrintLastError();
    }

   /* sockaddr_in fromAddress{};
    int fromSize = sizeof(fromAddress);
    const auto recivedSize = recvfrom(mySocket, messageBuffer, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&fromAddress), &fromSize);
    std::string aMessagem(messageBuffer);
    if (recivedSize == SOCKET_ERROR)
    {
        PrintLastError();
    }
    else if (aMessagem.find("Connect") != std::string::npos)
    {
        std::cout << "User has connected" << std::endl;
        char message[3] = "OK";
        const auto result = sendto(mySocket, message, Net::MessageBuffer, 0, reinterpret_cast<sockaddr*>(&fromAddress), sizeof(fromAddress));
        if (result == SOCKET_ERROR)
        {
            PrintLastError();
        }
    }*/
    while (true) 
    {
        aServer.UpdateTimer();
        aServer.Listen();
        aServer.UpdateGame();
    }

    aServer.DisconnectAllUsers();

    closesocket(mySocket);
    WSACleanup();

    system("PAUSE");
}
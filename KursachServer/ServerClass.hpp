#pragma once
#ifndef _SERVERCLASS_HPP_
#define _SERVERCLASS_HPP_
#define WIN32_LEAN_AND_MEAN

#define _GUI_CLIENT_

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <bitset>
#include <sstream>

class ServerClass
{
public:
    ServerClass();
    int SetUpWSA();
    int AddrInfo(const char* name);
    SOCKET Socket();
    int Bind();
    int Listen(int channels);
    SOCKET Accept();
    void Chat();
    int ShutDown();
    void ClearUp();
    ~ServerClass();
private:
    std::vector<std::string> commands;
    void Shorten(char recvBuffer[512]);
    std::string GetTime();
    bool CheckIfRegEx(char recvBuffer[512]);
    std::string Solve(char sendBuffer[512]);
    std::string CheckCommand(char recvBuffer[512]);
    std::string GetNumberSystem(char recvBuffer[512]);

    WORD version = MAKEWORD(2, 2);
    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = nullptr;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;

    int result = -1;
    static int count;
};

#endif

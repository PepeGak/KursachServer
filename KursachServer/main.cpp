#include "ServerClass.hpp"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char* args[])
{
    ServerClass server;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    int result;

    result = server.SetUpWSA();
    if (result != 0)
    {
        cout << "WSAStartup error " << result << "\n";
        return 1;
    }

    result = server.AddrInfo("666");
    if (result != 0)
    {
        cout << "getaddrinfo error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    ListenSocket = server.Socket();
    if (ListenSocket == INVALID_SOCKET)
    {
        cout << "socket error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    result = server.Bind();
    if (result == SOCKET_ERROR)
    {
        cout << "bind error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    result = server.Listen(SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        cout << "listen error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    ClientSocket = server.Accept();
    if (ClientSocket == INVALID_SOCKET)
    {
        cout << "accept error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    server.Chat();

    result = server.ShutDown();
    if (result == SOCKET_ERROR)
    {
        cout << "shutdown error " << result << "\n";
        server.ClearUp();
        return 1;
    }

    server.ClearUp();
    return 0;
}
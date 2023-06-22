#include "ServerClass.hpp"

int ServerClass::count = 0;
ServerClass::ServerClass()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass ctor()\n";
#endif

    if (ServerClass::count == 1)
    {
        std::cout << "Только 1 сервер может работать\n";
        exit(-1);
    }
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::ifstream fin;
    fin.open("Commands.txt");
    std::string command;
    while (fin.good())
    {
        std::getline(fin, command);
        this->commands.push_back(command);
    }
    fin.close();

    ZeroMemory(&this->hints, sizeof(this->hints));
    this->hints.ai_family = AF_INET;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_protocol = IPPROTO_TCP;
    this->hints.ai_flags = AI_PASSIVE;

    ServerClass::count++;
}

int ServerClass::SetUpWSA()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass SetUpWSA()\n";
#endif

    this->result = WSAStartup(this->version, &this->wsaData);
    return this->result;
}

int ServerClass::AddrInfo(const char* name)
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass AddrInfo()\n";
#endif

    this->result = getaddrinfo(NULL, name, &this->hints, &this->addrResult);
    return this->result;
}

SOCKET ServerClass::Socket()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass Socket()\n";
#endif

    this->ListenSocket = socket(this->addrResult->ai_family, this->addrResult->ai_socktype, this->addrResult->ai_protocol);
    return this->ListenSocket;
}

int ServerClass::Bind()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass Bind()\n";
#endif

    this->result = bind(this->ListenSocket, this->addrResult->ai_addr, static_cast<int>(this->addrResult->ai_addrlen));
    return this->result;
}

int ServerClass::Listen(int channels)
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass Listen()\n";
#endif

    std::cout << "Listening for clients...\n";
    this->result = listen(this->ListenSocket, channels);
    return this->result;
}

SOCKET ServerClass::Accept()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass Accept()\n";
#endif

    this->ClientSocket = accept(this->ListenSocket, nullptr, nullptr);
    return this->ClientSocket;
}

void ServerClass::Chat()
{
#ifdef _SERVERDEBUG_
    std::cout << "void ServerClass::Chat()\n";
#endif
    
    char recvBuffer[512];
    char sendBuffer[512];
    bool chatting = true;

    std::cout << "Connection esatblished. Enjoy your chat. To exit, send \'#\'\n";
    do
    {
        ZeroMemory(sendBuffer, sizeof(sendBuffer));
        ZeroMemory(recvBuffer, sizeof(recvBuffer));

        this->result = recv(this->ClientSocket, recvBuffer, sizeof(recvBuffer), 0);
#ifdef _SERVERDEBUG_
        std::cout << "ServerClass Chat::recv()\n";
#endif
#ifdef _GUI_CLIENT_
        this->Shorten(recvBuffer);
#endif

        if (this->result > 0)
        {
            std::cout << this->GetTime() << "Client: " << recvBuffer << "\n";
            if (!strcmp("#", recvBuffer))
            {
                std::cout << "Connection terminated\n";
                chatting = false;
                break;
            }
        }
        else if (this->result == 0)
        {
            std::cout << "Connection terminated\n";
            chatting = false;
            break;
        }
        else
        {
            std::cout << "recv error " << this->result << "\n";
            chatting = false;
            break;
        }

        std::cout << this->GetTime() << "Server: ";
        std::string com = this->CheckCommand(recvBuffer);
        if (com == "-1")
            std::cin.getline(sendBuffer, sizeof(sendBuffer));
        else if (com == "Bye")
        {
            std::cout << "Bye!\n";

            strcpy(sendBuffer, "#");
            strcpy(recvBuffer, "#");
        }
        else if (com == "Say ")
        {
            strcpy(sendBuffer, recvBuffer + 4);
            std::cout << sendBuffer << "\n";
        }
        else if (com == "Where are you")
        {
            std::cout << "I am here\n";

            GetModuleFileNameA(NULL, sendBuffer, sizeof(sendBuffer));
        }
        else if (com == "Solve ")
        {
            std::cout << "Solving...\n";

            std::string solvedReg = "The answer is ";
            solvedReg += this->Solve(recvBuffer + 6);
            strcpy(sendBuffer, solvedReg.c_str());
        }
        else if (com == "Notation of ")
        {
            std::cout << "Calculating...\n";

            std::string res = this->GetNumberSystem(recvBuffer + 12);
            strcpy(sendBuffer, res.c_str());
        }
        
        this->result = send(this->ClientSocket, sendBuffer, sizeof(sendBuffer), 0);
#ifdef _SERVERDEBUG_
        std::cout << "ServerClass Chat::send()\n";
#endif

        if (this->result == SOCKET_ERROR)
        {
            std::cout << "send error " << this->result << "\n";
            chatting = false;
            break;
        }
        if (!strcmp(sendBuffer, "#"))
        {
            std::cout << "Connection terminated...\n";
            chatting = false;
            break;
        }

    } while (chatting);

}

int ServerClass::ShutDown()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass ShutDown()\n";
#endif
    this->result = shutdown(this->ClientSocket, SD_SEND);
    return this->result;
}

void ServerClass::ClearUp()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass ClearUp()\n";
#endif

    if (this->addrResult != nullptr)
        freeaddrinfo(this->addrResult);
    this->addrResult = nullptr;

    if (this->ListenSocket != INVALID_SOCKET)
        closesocket(this->ListenSocket);
    this->ListenSocket = INVALID_SOCKET;

    if (this->ClientSocket != INVALID_SOCKET)
        closesocket(this->ClientSocket);
    this->ClientSocket = INVALID_SOCKET;

    WSACleanup();
}

ServerClass::~ServerClass()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass dtor()\n";
#endif
    SetConsoleCP(866);
    SetConsoleOutputCP(866);
}

#include "ServerClass.hpp"

void ServerClass::Shorten(char recvBuffer[512])
{
    std::string c = recvBuffer;
    int found = c.find_first_of("C");
    c.erase(0, static_cast<size_t>(found) + 8);
    ZeroMemory(recvBuffer, 512);
    CopyMemory(recvBuffer, c.c_str(), c.size());
}

std::string ServerClass::GetTime()
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass GetTime()\n";
#endif

    SYSTEMTIME t;
    GetLocalTime(&t);
    auto time = (t.wDay < 10 ? "0" : "") + std::to_string(t.wDay) + ".";
    time += (t.wMonth < 10 ? "0" : "") + std::to_string(t.wMonth) + ".";
    time += std::to_string(t.wYear) + " ";
    time += (t.wHour < 10 ? "0" : "") + std::to_string(t.wHour) + ":";
    time += (t.wMinute < 10 ? "0" : "") + std::to_string(t.wMinute) + ":";
    time += (t.wSecond < 10 ? "0" : "") + std::to_string(t.wSecond) + " ";
    return time;
}

std::string ServerClass::CheckCommand(char recvBuffer[512])
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass CheckCommand()\n";
#endif

    for (int i = 0; i < this->commands.size(); i++)
    {
        char* command = new char[this->commands[i].size() + 1];
        ZeroMemory(command, this->commands[i].size() + 1);
        CopyMemory(command, recvBuffer, this->commands[i].size());

        std::string comCheck = command;
        delete[] command;

        if (comCheck == this->commands[i])
            return this->commands[i];
    }
    return "-1";
}

bool ServerClass::CheckIfRegEx(char recvBuffer[512])
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass CheckIfRegEx()\n";
#endif

    for (int i = 0; recvBuffer[i] != '\0'; i++)
        if (
            (recvBuffer[i] < '0' || recvBuffer[i] > '9') &&
            (recvBuffer[i] != '+' && recvBuffer[i] != '-' &&
                recvBuffer[i] != '/' && recvBuffer[i] != '*' &&
                recvBuffer[i] != ' '))
            return false;
    return true;
}
std::string ServerClass::Solve(char sendBuffer[512])
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass Solve()\n";
#endif

    std::list<std::string> l;
    std::string lexeme;
    //Разбивка строки на лексемы
    for (int i = 0; sendBuffer[i] != '\0'; i++)
    {
        if (sendBuffer[i] >= '0' && sendBuffer[i] <= '9')
            lexeme += sendBuffer[i];
        else if (sendBuffer[i] == ' ')
            continue;
        else
        {
            if (i == 0 && sendBuffer[i] == '-')
            {
                lexeme += sendBuffer[i];
                continue;
            }
            l.push_back(lexeme);
            std::string symbol;
            switch (sendBuffer[i])
            {
            case '+':
                symbol = "+";
                break;

            case '-':
                symbol = "-";
                break;

            case '*':
                symbol = "*";
                break;

            case '/':
                symbol = "/";
                break;

            default:
                break;
            }

            l.push_back(symbol);
            lexeme.clear();
        }
    }
    l.push_back(lexeme);
    //Выбор приоритетов

    std::list<std::list<std::string>::iterator> lIter;
    for (auto i = l.begin(); i != l.end(); i++)
    {
        if (*i == "-" || *i == "+")
            lIter.push_back(i);
        else if (*i == "*" || *i == "/")
            lIter.push_front(i);
    }

    for (auto i = lIter.begin(); i != lIter.end(); i++)
    {
        auto t = *i;
        char oper = (*t)[0];
        double op1 = 1., op2 = 1., result = 1.;

        std::string number = *(--t);
        op1 = std::stod(number);
        ++(++t);
        number = *t;
        op2 = std::stod(number);

        switch (oper)
        {
        case '*':
            result = op1 * op2;
            break;

        case '/':
            result = op1 / op2;
            break;

        case '+':
            result = op1 + op2;
            break;

        case '-':
            result = op1 - op2;
            break;

        default:
            break;
        }

        t = l.erase(--t);
        t = l.erase(--t);
        t = l.erase(t);
        l.insert(t, std::to_string(result));
    }

    lIter.clear();
    auto back = std::stod(l.back());
    l.clear();

    lexeme = std::to_string(back);
    return lexeme;
}

std::string ServerClass::GetNumberSystem(char recvBuffer[512])
{
#ifdef _SERVERDEBUG_
    std::cout << "ServerClass GetNumberSystem()\n";
#endif

    std::string numberStr = recvBuffer;
    std::string result = "", tRes = "";

    long n = std::stol(numberStr);
    while (n != 0)
    {
        int rem = 0;
        char ch = 0;
        rem = n % 16;
        ch = rem;
        if (rem < 10)
            ch += 48;
        else
            ch += 55;
        tRes += ch;
        n /= 16;
    }
    std::reverse(tRes.begin(), tRes.end());
    result += "HEX: ";
    result += tRes;
    result += " ";
    tRes.clear();

    n = std::stol(numberStr);
    while (n != 0)
    {
        int rem = 0;
        char ch = 0;
        rem = n % 8;
        ch = rem;
        ch += 48;
        tRes += ch;
        n /= 8;
    }
    std::reverse(tRes.begin(), tRes.end());
    result += "OCT: ";
    result += tRes;
    result += " ";
    tRes.clear();

    n = std::stol(numberStr);
    while (n != 0)
    {
        int rem = 0;
        char ch = 0;
        rem = n % 2;
        ch = rem;
        ch += '0';
        tRes += ch;
        n /= 2;
    }
    std::reverse(tRes.begin(), tRes.end());
    result += "BIN: ";
    result += tRes;
    tRes.clear();

    return result;
}

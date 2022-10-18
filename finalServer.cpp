#include "Socket.hpp"
#include <thread>
#include <vector>

void printIncomingData(Socket::conn* conn)
{
    std::string str;
    while (true)
    {
        if(conn->connRecv(str) < 0)
        {
            break;
        }
        std::cout << str << std::endl;
    }
    
}

int main()
{
    Socket sock("127.0.0.1", 6969);

    sock.setSocketToListen(0);

    Socket::conn newConn = sock.acceptConn();

    auto t = std::thread(printIncomingData, &newConn);

    std::string str;

    while (true)
    {
        std::getline(std::cin, str);
        if(newConn.connSend(str) < 0)
        {
            break;
        }
    }
    

    return 0;
}
#include "Socket.h"
#include <thread>

void printIncomingData(Socket::conn* conn)
{
    std::string str;
    while (true)
    {
        conn->connRecv(str);
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
        newConn.connSend(str);
    }
    

    return 0;
}
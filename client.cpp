#include "Socket.hpp"
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

    Socket::conn newConn = sock.connectToServer();

    if(newConn.connfd < 0)
    {
        return -1;
    }

    auto t = std::thread(printIncomingData, &newConn);

    std::string str;

    while (true)
    {
        std::getline(std::cin, str);
        newConn.connSend(str);
    }

    return 0;
}
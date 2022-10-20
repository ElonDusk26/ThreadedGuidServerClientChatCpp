#include "Socket.hpp"
#include <thread>
#include <vector>
#include <utility>

class Server
{
    private:
    Socket::conn conns[16] = {}; //connfd of -1 equals unused
    std::thread threads[16] = {};

    Socket* serverSock;

    int getEmptyConnIndex()
    {
        for (int i = 0; i < sizeof(conns); i++)
        {
            if(conns[i].connfd == -1)
            {
                return i;
            }
        }
        return -1;
    }

    void connRoutine(int index)
    {
        std::cout << "started new thread with id " << std::this_thread::get_id() << std::endl;
        std::string recvString;
        while(true)
        {
            if(conns[index].connRecv(recvString) < 0)
            {
                //failed to receive msg
                std::cout << "lost connection\n";
                break;
            }
            std::cout << "sending message to all conns: " << recvString << std::endl;
            sendMsgToAllConns(recvString, index);
        }
        conns[index].~conn(); //call destructor clearing the socket
        memset(&conns[index], -1, sizeof(conns[index])); //empty the element of the array. Ready for next conn
        std::cout << "freed conn index " << index << std::endl;
        std::cout << "terminating thread with id " << std::this_thread::get_id() << std::endl;
        threads[index].detach(); //detach thread from main thread. Gets deleted once out of scope
        memset(&threads[index], 0, sizeof(std::thread)); //clear array mem
    }

    void sendMsgToAllConns(std::string msg, int index)
    {
        for(int i; i < std::size(conns); i++)
        {
            if(i != index && conns[i].connfd != -1)
            {
                conns[i].connSend(msg); //needs error handling
            }
        }
    }

    public:
    int setupServer(const char* ip, int port)
    {
        serverSock = new Socket(ip, port);

        if(!serverSock->status)
        {
            return -1;
        }
        if(serverSock->setSocketToListen(0) < 0)
        {
            return -1;
        }
        return 0;
    }

    void acceptConnThread()
    {
        int emptyIndex = getEmptyConnIndex();
        if(emptyIndex < 0)
        {
            std::cout << "cannot accept more connections\n";
            return;
        }

        if(serverSock->acceptConn(conns[emptyIndex]) < 0)
        {
            std::cout << "error accepting connection\n";
            return;
        }

        std::cout << "accepted new connection with handle " << conns[emptyIndex].connfd << std::endl;

        threads[emptyIndex] = std::thread(&Server::connRoutine, this, emptyIndex);
    }

};




int main()
{
    Server serv;

    serv.setupServer("127.0.0.1", 6969);

    while (true)
    {
        serv.acceptConnThread();
    }
    
    return 0;
}
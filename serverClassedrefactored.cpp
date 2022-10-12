#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


class Socket
{
    int socketfd;
    struct sockaddr_in serverInfo = {};
    socklen_t serverInfoLen = sizeof(serverInfo);

    public:

    struct conn
    {
        int connfd;
        char buff[1024] = {};

        int connSend(std::string string)
        {
            memset(&buff, 0, sizeof(buff));
            strcpy(buff, string.c_str());
            int res = send(connfd, &buff, sizeof(buff), 0);
            if(res < 0)
            {
                perror("error sending data");
            }
            return res;
        }
        int connRecv(std::string &string)
        {
            memset(&buff, 0, sizeof(buff));
            int res = recv(connfd, &buff, sizeof(buff), 0);
            if(res < 0)
            {
                perror("error receiving data");
                return -1;
            }
            string = std::string(buff); //type cast char into string;
            return res;
        }
        ~conn()
        {
            std::cout << "exiting conn correctly\n";
            close(connfd);
        }
    };

    bool status = true;
    Socket(std::string ip, int port)
    {
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());
        serverInfo.sin_port = htons(port);
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketfd < 0)
        {
            perror("error creating socket");
            bool status = false;
        } 
    }
    ~Socket()
    {
        close(socketfd);
    }
    
    int setSocketToListen(int listenQueue)
    {
        int res = bind(socketfd, (sockaddr*)&serverInfo, serverInfoLen);
        if(res < 0)
        {
            perror("error binding socket");
            return -1;
        }

        res = listen(socketfd, listenQueue);
        if(res < 0)
        {
            perror("error listening to socket");
            return -1;
        }
        return 0;
    }

    conn connectToServer()
    {
        conn connOut;
        connOut.connfd = (socketfd, (sockaddr*)&serverInfo, serverInfoLen);
        if(connOut.connfd < 0)
        {
            perror("error connecting to server");
        }
    }

    conn acceptConn()
    {
        conn connOut;
        connOut.connfd = accept(socketfd, NULL, NULL);
        return connOut;
    }

};

int main()
{
    Socket sock("127.0.0.1", 6969);

    sock.setSocketToListen(0);

    Socket::conn newConn = sock.acceptConn();

    newConn.connSend("Hello\n");

    std::string recvString;

    newConn.connRecv(recvString);

    std::cout << recvString << std::endl;

    return 0;
}
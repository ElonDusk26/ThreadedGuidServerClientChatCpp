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


class SocketWrapper
{
    public:
    struct sock
    {
        int socketfd;
        struct sockaddr_in serverInfo = {};
        socklen_t serverInfoLen = sizeof(serverInfo);
        ~sock()
        {
            close(socketfd);
        }
    };
    struct conn
    {
        int connfd;
        char buff[1024] = {};

        int send()
    };

    static int configureSocket(sock &socketIn, std::string ip, int port)
    {
        socketIn.serverInfo.sin_family = AF_INET;
        socketIn.serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());
        socketIn.serverInfo.sin_port = htons(port);
        socketIn.socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketIn.socketfd < 0)
        {
            perror("error creating socket");
            return -1; 
        }
        return 0;
    }
    static int setSocketToListen(sock &socketIn, int listenQueue)
    {
        int res = bind(socketIn.socketfd, (sockaddr*)&socketIn.serverInfo, socketIn.serverInfoLen);
        if(res < 0)
        {
            perror("error binding socket");
            return -1;
        }

        res = listen(socketIn.socketfd, listenQueue);
        if(res < 0)
        {
            perror("error listening to socket");
            return -1;
        }
        return 0;
    }
};

int main()
{
    SocketWrapper::sock* serverSocket = new SocketWrapper::sock;

    SocketWrapper::configureSocket(*serverSocket, "127.0.0.1", 6969);

    SocketWrapper::setSocketToListen(*serverSocket, 0);

    int connfd = accept(serverSocket->socketfd, NULL, NULL);
    if(connfd < 0)
    {
        perror("error accepting connection");
        return -1;
    }

    char buff[1024]={};

    strcpy(buff, "Hello\n");

    send(connfd, buff, sizeof(buff), 0);

    memset(&buff, 0, sizeof(buff));

    recv(connfd, &buff, sizeof(buff), 0);

    std::cout << buff << std::endl;

    delete serverSocket;


    return 0;
}
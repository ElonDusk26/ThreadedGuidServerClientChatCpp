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
    struct sock
    {
        int socketfd;
        struct sockaddr_in serverInfo = {0};
        socklen_t serverInfoLen = sizeof(serverInfo);
    };

    int configureSocket(sock &socketIn, std::string ip, int port)
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
    int setSocketToListen(sock &socketIn, int listenQueue)
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
    int socketfd = 0;
    struct sockaddr_in serv_addr = {0};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(6969);

    socklen_t servAddrLen = sizeof(serv_addr);

    
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("error creating socket");
        return -1; 
    }

    int res = bind(socketfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    if(res < 0)
    {
        perror("error binding socket");
        return -1;
    }

    res = listen(socketfd, 10);
    if(res < 0)
    {
        perror("error listening to socket");
        return -1;
    }

    char buff[1024];
    memset(&buff, 0, sizeof(buff));
  
    struct sockaddr clientInfo;

    socklen_t clientInfoSize = sizeof(clientInfo);

    int connfd = accept(socketfd, (struct sockaddr*)&clientInfo, &clientInfoSize);
    if(connfd < 0)
    {
        perror("error accepting connection");
        return -1;
    }

    strcpy(buff, "Hello\n");

    send(connfd, buff, sizeof(buff), 0);

    memset(&buff, 0, sizeof(buff));

    recv(connfd, &buff, sizeof(buff), 0);

    std::cout << buff << std::endl;

    close(connfd);


    return 0;
}
#pragma once
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
    int socketfd; //socket file descriptor
    struct sockaddr_in serverInfo = {};
    socklen_t serverInfoLen = sizeof(serverInfo);

    public:
    bool status = true;

    struct conn
    {
        int connfd = -1;
        int connSend(std::string &string);
        int connRecv(std::string &string);
        ~conn();
    };

    Socket(std::string ip, int port);

    ~Socket();
    
    int setSocketToListen(int listenQueue);

    conn connectToServer();

    conn acceptConn();

    int acceptConn(conn& inConn);
};

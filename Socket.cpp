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
#include "Socket.hpp"

Socket::Socket(std::string ip, int port) 
{
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());
    serverInfo.sin_port = htons(port);
    socketfd = socket(AF_INET, SOCK_STREAM, 0); //creating socket
    if (socketfd < 0)
    {
        perror("error creating socket");
        status = false;
    } 
}

Socket::~Socket()
{
    close(socketfd);
}

int Socket::setSocketToListen(int listenQueue)
{
    int res = bind(socketfd, (sockaddr*)&serverInfo, serverInfoLen); //bind socket to ip and port
    if(res < 0)
    {
        perror("error binding socket");
        return -1;
    }

    res = listen(socketfd, listenQueue); //start listening to new connections
    if(res < 0)
    {
        perror("error listening to socket");
        return -1;
    }
    return 0;
}

Socket::conn Socket::connectToServer()
{
    conn connOut;
    connect(socketfd, (sockaddr*)&serverInfo, serverInfoLen); //connect to target server
    if(socketfd < 0)
    {
        perror("error connecting to server");
    }
    connOut.connfd = socketfd; //bad code: sets the connectionfd to the socket fd. 
    return connOut;
}

Socket::conn Socket::acceptConn()
{
    conn connOut;
    connOut.connfd = accept(socketfd, NULL, NULL); //accept incoming connection
    return connOut;
}

int Socket::acceptConn(conn& inConn)
{
    inConn.connfd = accept(socketfd, NULL, NULL); //accept incoming connection
    return inConn.connfd;
}

Socket::conn::~conn()
{
    close(connfd);
}

int Socket::conn::connRecv(std::string &string)
{
    string.clear();
    int size = 0;
    int res = recv(connfd, &size, sizeof(int), 0); //get the size of the incoming string
    if(res <= 0)
    {
        perror("error receiving data");
        return -1;
    }
    
    //std::cout << "receiving string with a size of " << size << std::endl;

    auto buffer = std::make_unique<char[]>(size);//new char[size]; //allocate buffer with the size of the incoming data

    res = recv(connfd, buffer.get(), size, 0); //fill the buffer
    if(res <= 0)
    {
        perror("error receiving data");
        return -1;
    }
    string = std::string(buffer.get(), size); //set return string to the buffer

    return res;
}

int Socket::conn::connSend(std::string &string)
{
    auto buffer = string.c_str();
    int size = string.size();

    //std::cout << "sending " << string << " with an allocated buffer of " << size << std::endl;

    int res = send(connfd, &size, sizeof(int), 0); //send size of the string to be sent
    if(res <= 0)
    {
        perror("error sending data");
        return -1;
    }
    res = send(connfd, buffer, size, 0); //send the string
    if(res <= 0)
    {
        perror("error sending data");
        return -1;
    }
    return res;
}
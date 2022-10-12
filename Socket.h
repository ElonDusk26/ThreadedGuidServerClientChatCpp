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
    int socketfd; //socket file descriptor
    struct sockaddr_in serverInfo = {};
    socklen_t serverInfoLen = sizeof(serverInfo);

    public:

    struct conn
    {
        int connfd;
        char buff[1024] = {}; //reused buffer

        int connSend(std::string string)
        {
            memset(&buff, 0, sizeof(buff)); //reset buffer
            strcpy(buff, string.c_str());
            int res = send(connfd, &buff, sizeof(buff), 0); //send data using target file descriptor
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
            close(connfd);
        }
    };

    Socket(std::string ip, int port)
    {
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_addr.s_addr = inet_addr(ip.c_str());
        serverInfo.sin_port = htons(port);
        socketfd = socket(AF_INET, SOCK_STREAM, 0); //creating socket
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

    conn connectToServer()
    {
        conn connOut;
        connect(socketfd, (sockaddr*)&serverInfo, serverInfoLen); //connect to target server
        if(connOut.connfd < 0)
        {
            perror("error connecting to server");
        }
        connOut.connfd = socketfd; //bad code: set the connectionfd to the socket fd. 
        return connOut;
    }

    conn acceptConn()
    {
        conn connOut;
        connOut.connfd = accept(socketfd, NULL, NULL); //accept incoming connection
        return connOut;
    }

};
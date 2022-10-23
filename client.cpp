#include "Socket.hpp"
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

char getch(bool block = false) {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");

        if(!block)
        {
            int flags = fcntl(0, F_GETFL, 0); //makes sure it dosent block
            fcntl(0, F_SETFL, flags | O_NONBLOCK);
        }

        if (read(0, &buf, 1) < 0)
                //perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

std::string inputString;

void printIncomingData(Socket::conn* conn)
{
    std::string str;
    while (true)
    {
        if(conn->connRecv(str) < 0)
        {
            break;
        }
        //overwrite the written line

        std::cout << "\r\e[K";

        //write new line and set the next line to the already written line.

        std::cout << "\r" << str << std::endl << inputString << std::flush;
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

    while (true)
    {
        char newChr = getch(true);
        switch (newChr)
        {
            case '\n': //break at newline to send the inputString
                break;
            case '\x7f': //backspace
                if(inputString.length() > 0) //checks if the length is over 0 (so it dosent pop an empty string)
                {
                    std::cout << "\r\e[K"; //clear terminal
                    inputString.pop_back(); //remove last letter
                    std::cout << "\r" << inputString << std::flush; //write the new line
                }
                continue;
            default:
                std::cout << newChr << std::flush; //write the new character to the screen
                inputString.push_back(newChr); //add to the string
                continue;
        }
        
        if(newConn.connSend(inputString) < 0)
        {
            break;
        }
        inputString.clear();
        std::cout << std::endl;
    }

    return 0;
}
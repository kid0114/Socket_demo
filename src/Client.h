//
// Created by kid on 5/21/22.
//

#ifndef DEMO_CLIENT_H
#define DEMO_CLIENT_H
#define  PORT  6001
#define  MAXLINES 2048
#define  LINES 200 // the number of lines and the connection tries
#include <arpa/inet.h>
#include <cstring>

class Client {
public:
    int clientfd;
    int serverfd;
    int len;
    struct sockaddr_in servaddr,cliaddr;
    int port;
    int connectx;
    char servbuf[MAXLINES];
    char clibuf[MAXLINES];

    int threadheart();

    void *heartbeat();
    int CommuServer1();
    int CommuServer2();
    char cmdx[100];
    Client(char *ip,int port);
    void readserver();
    void sendserver();
    int checkcmd();
    int ncmd;
};


#endif //DEMO_CLIENT_H

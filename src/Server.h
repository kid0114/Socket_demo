//
// Created by kid on 5/21/22.
//

#ifndef DEMO_SERVER_H
#define DEMO_SERVER_H

#define  PORT  6001
#define  MAXLINES 2048
#include <thread>
#include <arpa/inet.h>
using namespace std;
/*
 * struct sockaddr_un{
    sa_family_t sun_family;//AF_UNIX，它是一个短整型
    char        sum_path[];//路径名
};

对于AF_INET域套接字来说，它的地址结构由sockaddr_in来描述，它至少包括以下几个成员：

struct sockaddr_in{
    short int            sin_family;//AF_INET
    unsigned short int    sin_port;//端口号
    struct in_addr        sin_addr;//IP地址
};

而in_addr被定义为：
[cpp] view plain copy
struct in_addr{
    unsigned long int s_addr;
};

 *
 */
#define SA sockaddr
#define LISTENN 40
class Server{
public:
    int serverfd;
    int clientfd;
    struct sockaddr_in servaddr,cliaddr;
    char servbuf[MAXLINES];//server data
    char clibuf[MAXLINES];// client data
    //char cmdx[100];
    ///int  ncmd;
    int port;
    int listenx;
    int bindx;
    int len;

    time_t  ticks;
    Server(char *ip,int x);//prepare listen

    //connection information
    void connect_print();

    //connection init types
    int try_connect();//single connection

    //multiple processes/threads
    int multip_connect();//process
    int multit_connect();//thread

    //events play
    int epoll_connect();//epoll
    int select_connect();//select
    int poll_connect();//poll

    // read
    void readclient();
    void sendclient();
    int CommuClient1();
    int CommuClient2();

    int checkcmd();
};



#endif //DEMO_SERVER_H

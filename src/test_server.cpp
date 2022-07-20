//
// Created by kid on 5/21/22.
//
#include <iostream>
#include "Server.h"
#include "Client.h"
char ip[]="192.168.75.129";
int main() {
    //std::cout << "Hello, World!" << std::endl;

    Server A(ip,6001);
    A.epoll_connect();
    //A.try_connect();
    //A.select_connect();
    //A.poll_connect();
    //A.multip_connect();
    return 0;
}

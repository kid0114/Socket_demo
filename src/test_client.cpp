//
// Created by kid on 5/21/22.
//
#include <iostream>
#include "Server.h"
#include "Client.h"
char ip[]="192.168.75.129";
int main() {
    //std::cout << "Hello, World!" << std::endl;
    Client B(ip,6001);

    return 0;
}

//
//  TCPClient.hpp
//  Grab
//
//  Created by 周伟杰 on 2021/10/22.
//
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>
//#include "../Agent/Base.h"
 
using namespace std;
 
class TCPClient
{
  private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
 
  public:
    TCPClient();
    bool setup(string address, int port);
    bool Send(string data);
    string receive(int size = 4096);
    string read();
    void exit();
};
 
#endif

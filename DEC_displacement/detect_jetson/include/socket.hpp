//
//  socket.hpp
//  DEC_robotic
//
//  Created by 周伟杰 on 2021/8/11.
//

#ifndef socket_hpp
#define socket_hpp

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <opencv2/opencv.hpp>

#define BUF_SIZE 1024

using namespace std;

class Server{
public:
    Server();
    Server(string ippad, int portad);
    ~Server();
    
    int serverLink();
    void serverListen();
    string serverAccept();
    void serverSend(cv::Point centerPoint);
    void serverClose();
    
public:
    int  listenfd, connfd;
    struct sockaddr_in  servaddr;
};


class Client{
public:
    Client();
    ~Client();
    
    void clientLink();
    void clientWrite(string buf);
    void clientWrite(float angle);
    string clientRead();
    void clientClose();
    
public:
    char *IP = "127.0.0.1";
    unsigned int SERV_PORT = 9999;
    int sockfd;
    struct sockaddr_in s_addr;
    socklen_t len;
    char buf[];
    
    
    
};
#endif /* socket_hpp */

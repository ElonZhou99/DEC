//
//  socket.cpp
//  DEC_robotic
//
//  Created by 周伟杰 on 2021/8/11.
//

#include "socket.hpp"




Server::Server() {
    
}

Server::Server(string ippad, int portad) {

}

Server::~Server() {
    
}

int Server::serverLink() {
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(6666);
    if(::bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    return 0;
}

void Server::serverListen() {
    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    }
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    if( connfd == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
    }
    printf("connected!\n");
    
}


string Server::serverAccept() {
    char recvbuff[BUF_SIZE];
    //sprintf(sendbuff, "xiba");
    //send(connfd, sendbuff, sizeof(sendbuff), 0);
    printf("msg has been sent!\n");
    int iRet = recv(connfd, recvbuff, sizeof(recvbuff), 0);
    if(iRet <= 0){
        //printf("connection stopped.\n");
        return "no change";
    }
    printf("recv msg from client: %s\n", recvbuff);
    printf("msg has been received!\n");
    return recvbuff;
}

void Server::serverSend(cv::Point centerPoint) {

    string poseX = to_string(centerPoint.x);
    string poseY = to_string(centerPoint.y);
    string pose = "x:" + poseX + "   " + "y:" + poseY;
    string* news = &pose;
    send(connfd, news, sizeof(*news), 0);
    //memset(buffer, 0, BUF_SIZE);  //重置缓冲区
     
}

void Server::serverClose() {
    close(listenfd);
    //close(serv_sock);
}





Client::Client() {
    
}

Client::~Client() {
    
}

void Client::clientLink() {
    unsigned int port;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }

    port = SERV_PORT;

    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);

    if(inet_aton(IP, (struct in_addr*)&s_addr.sin_addr.s_addr) == 0){
        perror("IP error");
        exit(errno);
    }

    /*开始连接服务器*/
    if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(errno);
    }else
        printf("*****************client start***************\n");
}


void Client::clientWrite() {
    string req = "request";
    string *request = &req;
    send(sockfd, request, sizeof(*request), 0);
}


void Client::clientWrite(cv::Point centerPoint) {
    
    string poseX = to_string(centerPoint.x);
    string poseY = to_string(centerPoint.y);
    string pose = "x:" + poseX + "   " + "y:" + poseY;
    string* news = &pose;
    send(sockfd, news, sizeof(*news), 0);
}



string Client::clientRead() {
    bzero(buf,BUF_SIZE);
    len = recv(sockfd,buf,BUF_SIZE,0);
    string bufstr = buf;
    if(len > 0) {
        string temp(bufstr, 0, 4);
        if (temp == "AABB"){
            string ans(bufstr, 4, 3);
            cout << "target color: " << ans;
            return ans;
        }
    }
    return "";
}


void Client::clientClose() {
    close(sockfd);
}

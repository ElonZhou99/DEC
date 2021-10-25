//
//  TCPClient.cpp
//  Grab
//
//  Created by 周伟杰 on 2021/10/22.
//

#include "TCPClient.hpp"
 
TCPClient::TCPClient(){
    sock = -1;
    port = 0;
    address = "";
}
 
bool TCPClient::setup(string address , int port){
      if(sock == -1)
    {
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
              cout << "Could not create socket" << endl;
        }
    }
      if(inet_addr(address.c_str()) == (unsigned)-1)
      {
        struct hostent *he;
        struct in_addr **addr_list;
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            herror("gethostbyname");
              cout<<"Failed to resolve hostname\n";
            return false;
        }
           addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            server.sin_addr = *addr_list[i];
            break;
        }
      }
      else
      {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
      }
      server.sin_family = AF_INET;
      server.sin_port = htons( port );
      if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
      {
            perror("connect failed. Error");
            return false;
      }
      return true;
}
 
bool TCPClient::Send(string data){
 
    cout<< "the data is："<< data <<endl;
 
    if(sock != -1) {
        int length = data.length();
        if(send(sock, &length, sizeof(int), 0 ) < 0 ){
            cout << "Send length failed : " << endl;
            return false;
        }
 
        if( send(sock , data.c_str() , data.length() , 0) < 0){
            cout << "Send failed : " << data << endl;
            return false;
        }
    }
    else
        return false;
    return true;
}
 
string TCPClient::receive(int size){
    int length = 0;
    recv(sock,&length,sizeof(int),0);
      char* message = (char*)malloc(length+1);
    memset(message, 0, length+1);
    recv(sock,message,length,0);
      string reply = message;
    free(message);
      return reply;
}
 
void TCPClient::exit()
{
    close( sock );
}

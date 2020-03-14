#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "cs.h"
#include "ErrCode.h"

using namespace std;

#define PORT 9977
#define MAX_LINE 20
#define MSG_SIZE 4096
#define DEFAULT_SERVER "49.233.211.29"

int client() {
    struct sockaddr_in serverAddr; // 一个将来与套接字绑定的结构体
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_port = (PORT); // 从客户端的PORT端口接收服务器信息
    serverAddr.sin_family = AF_INET; // 协议簇，AF_INET表示TCP/IP协议   
    cout << "Please input the server address: (输入1连接默认服务器)" << endl;
    char sAddr[20];
    cin >> sAddr;
    if (strcmp(sAddr, "1") == 0) {
        if(inet_pton(AF_INET, DEFAULT_SERVER, (void *)&serverAddr.sin_addr) <= 0) {
            cout << "client inet_pton err" << endl;
            return CLIENT_INETPTON_ERR;
        }
    }
    else {
        if(inet_pton(AF_INET, DEFAULT_SERVER, (void *)&serverAddr.sin_addr) <= 0) {
            cout << "client inet_pton err" << endl;
            return CLIENT_INETPTON_ERR;
        }
    }

    int fd_skt = socket(AF_INET, SOCK_STREAM, 0); // （1）socket函数新建套接字fd_skt
    if (fd_skt < 0) {
        cout << "client socket err" << fd_skt << endl;
        return CLIENT_SOCKET_ERR;
    }

    if (connect(fd_skt, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) { //（2）connect向服务器发起连接请求
        cout << "client connect err" << endl;
        return CLIENT_CONNECT_ERR;
    }

    cout << "Connect Success! \n Begin to chat..." << endl;

    while(1) {
        char cMsg[MSG_SIZE];
        cin >> cMsg;
        if(strcmp(cMsg, "quit")){
            break;
        }
        if(send(fd_skt, cMsg, strlen(cMsg), 0) < 0) { // （3）send，客户端向服务端发消息
            cout << "client send err" << endl;
            return CLIENT_SEND_ERR;
        }
        cout << "Send message to <" << serverAddr.sin_addr.s_addr << ">: " << cMsg << endl;
        char sMsg[MSG_SIZE];

        int sLen = recv(fd_skt, sMsg, sizeof(sMsg),0); // （4） recv，接收服务器发来的消息
        if(sLen <= 0) { 
            cout << "client recv err" << endl;
            return CLIENT_RECV_ERR;
        }
        sMsg[sLen] = '\0';
        cout << "Receive message form <" << serverAddr.sin_addr.s_addr << ">: " << sMsg << endl;

        memset(cMsg, 0, sizeof(cMsg));
        memset(sMsg, 0, sizeof(sMsg));
    }
    close(fd_skt);
}
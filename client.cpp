#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "cs.h"
#include "DefineCode.h"
#include "Des.h"

using namespace std;

#define PORT 9977
#define MAX_LINE 20
#define MSG_SIZE 4096
#define DEFAULT_SERVER "127.0.0.1"

int client() {
    CDesOperate des; // DES对象
    char cMsg[MSG_SIZE], sMsg[MSG_SIZE];

    struct sockaddr_in serverAddr; // 一个将来与套接字绑定的结构体
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_port = (PORT); // 从客户端的PORT端口接收服务器信息
    serverAddr.sin_family = AF_INET; // 协议簇，AF_INET表示TCP/IP协议   
    cout << "Please input the server address: (输入1连接默认服务器)" << endl;
    char sAddr[20];
    cin >> sAddr;
    if (strcmp(sAddr, "1") == 0) {
        if(inet_pton(AF_INET, DEFAULT_SERVER, (void *)&serverAddr.sin_addr) <= 0) {
            perror("client inet_pton err");
            return CLIENT_INETPTON_ERR;
        }
    }
    else {
        if(inet_pton(AF_INET, DEFAULT_SERVER, (void *)&serverAddr.sin_addr) <= 0) {
            perror("client inet_pton err");
            return CLIENT_INETPTON_ERR;
        }
    }

    int fd_skt = socket(AF_INET, SOCK_STREAM, 0); // （1）socket函数新建套接字fd_skt
    if (fd_skt < 0) {
        perror("client socket err");
        return CLIENT_SOCKET_ERR;
    }
    
    if (connect(fd_skt, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) { //（2）connect向服务器发起连接请求
	    perror("client connect err");       
        return CLIENT_CONNECT_ERR;
    }

    cout << "Connect Success! \nBegin to chat..." << endl;
    cin.ignore(1024,'\n'); // 去除上一个cin残留在缓冲区的\n
    while(1) {
        memset(cMsg, 0, sizeof(cMsg));
        memset(sMsg, 0, sizeof(sMsg));

	    cin.getline(cMsg, sizeof(cMsg)); // 不用cin，因为不能含空格
        if (strcmp(cMsg, "quit") == 0) {
            break;
        }
        
        cout << "Send message to <" << ntohl(serverAddr.sin_addr.s_addr) << ">: " << cMsg << endl;

        string encryResult; // 加密结果
        if (des.Encry(cMsg, DES_KEY, encryResult) != 0) { // 加密
            perror("encry err");
            return DES_ENCRY_ERR;
        }
        memset(cMsg, '\0', MSG_SIZE);
        for (int i = 0; i < encryResult.length(); i++) { // 加密结果string转char[]
            cMsg[i] = encryResult[i];
        }
        cMsg[encryResult.size()] = '\0';
        
        if (send(fd_skt, cMsg, strlen(cMsg), 0) < 0) { // （3）send，客户端向服务端发消息
            perror("client send err");
            return CLIENT_SEND_ERR;
        }
        
        int sLen = recv(fd_skt, sMsg, sizeof(sMsg),0); // （4） recv，接收服务器发来的消息
        if(sLen <= 0) { 
            perror("client recv err");
            break;
        }
        sMsg[sLen] = '\0';

        string decryResult = "";
        if (des.Decry(sMsg, DES_KEY, decryResult) != 0) { //解密
            perror("decry err");
            return DES_DECRY_ERR;
        }

        cout << "Receive message from <" << ntohl(serverAddr.sin_addr.s_addr) << ">: " 
             << decryResult << endl;
    }
    cout << "--- client end ---" << endl;
    close(fd_skt); // 服务器会recv err: SUCCESS，从而结束连接
}

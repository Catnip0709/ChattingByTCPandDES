#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "cs.h"
#include "DefineCode.h"
#include "Des.h"

using namespace std;

#define PORT 9977
#define MSG_SIZE 4096
#define MAX_LINE 20

int server() {
    CDesOperate des; // DES对象

    char cMsg[MSG_SIZE], sMsg[MSG_SIZE];

    struct sockaddr_in serverAddr; // 一个将来与套接字绑定的结构体
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_port = (PORT); // 从服务器的PORT端口接收客户端信息
    serverAddr.sin_family = AF_INET; // 协议簇，AF_INET表示TCP/IP协议
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收的信息来自于某个IP，这里随意

    int fd_skt = socket(AF_INET, SOCK_STREAM, 0); // （1）socket函数新建套接字fd_skt
    if (fd_skt < 0) {
        perror("server socket err");
        return SERVER_SOCKET_ERR;
    }

    if (bind(fd_skt, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0 ) { // （2）bind绑定套接字与serverAddr
        perror("server bind err");
        return SERVER_BIND_ERR;
    }
    if (listen(fd_skt, MAX_LINE) < 0) { // （3）listen监听套接字fd_skt
        perror("server listen err");
        return SERVER_LISTEN_ERR;
    }
    cout << "Listening..." << endl;

    while(1) {
        memset(cMsg, 0, sizeof(cMsg));
        memset(sMsg, 0, sizeof(sMsg));

        struct sockaddr_in clientAddr;
        socklen_t length = sizeof(clientAddr);
        int fd_client = accept(fd_skt, (struct sockaddr*)&clientAddr, &length);
        // （4）accept接收连接请求：程序在此阻塞直至产生第一次握手
        // 接收到的信息存于第二第三个参数中
        // 返回值是新的文件描述符，用于后续read和write调用

        if (fd_client < 0) {
            perror("server accept err");
            return SERVER_ACCEPT_ERR;
        }
        
        cout << "server: got connection from " << clientAddr.sin_addr.s_addr
             << ", port " << PORT
             << ", socket" << fd_client << endl;

        int cLen = recv(fd_client, cMsg, MAX_LINE, sizeof(cMsg)); // （5）read将接收到的客户端消息存在cMsg中
        if (cLen <= 0) {
            perror("server recv err");
            return SERVER_RECV_ERR;
        }
        cMsg[cLen] = '\0';
        cout << "Receive message form <" << clientAddr.sin_addr.s_addr << ">: " << cMsg << endl;

        cin.ignore(1024,'\n'); // 去除上一个cin残留在缓冲区的\n 
        cin.getline(sMsg, sizeof(sMsg)); // 不用cin，因为不能含空格
        if(strcmp(sMsg, "quit\n") == 0) {
            break;
        }

        string encryResult; // 加密结果
        des.Encry(sMsg, DES_KEY, encryResult); // 加密
        cout<<"encryResult = "<<encryResult<<endl;
        memset(sMsg, '\0', MSG_SIZE);
        for (int i = 0; i < encryResult.length(); i++) { // 加密结果string转char[]
            sMsg[i] = encryResult[i];
        }
        sMsg[encryResult.size()] = '\0';

        if(send(fd_client, sMsg, MAX_LINE, sizeof(sMsg)) <= 0) { // （6）send将服务器的消息发给客户端
            perror("server send err");
            return SERVER_SEND_ERR;
        }
        // recv和send的返回值：
        // >0表示成功，返回实际发送或接受的字节数
        // =0表示超时，对方主动关闭了连接过程
        // <0出错
        cout << "Send message to <" << clientAddr.sin_addr.s_addr << ">: " << sMsg << endl;

        close(fd_client);
    }
    close(fd_skt);
    return SUCCESS;
}

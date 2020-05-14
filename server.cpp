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
    
    while(1) {
        cout << "Listening..." << endl;

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
        
        cout << "server: got connection from " << ntohl(clientAddr.sin_addr.s_addr)
             << ", port " << PORT
             << ", socket" << fd_client << endl;
        
        cin.ignore(1024,'\n'); // 去除上一个cin残留在缓冲区的\n 
        while(1) { // 与当前客户端循环通信
            memset(cMsg, 0, sizeof(cMsg));
            memset(sMsg, 0, sizeof(sMsg));
            int cLen = recv(fd_client, cMsg, sizeof(cMsg), 0); // （5）read将接收到的客户端消息存在cMsg中
            if (cLen <= 0) {
                perror("server recv err");
                break;
            }
            cMsg[cLen] = '\0';

            string decryResult = "";
            if (des.Decry(cMsg, DES_KEY, decryResult) != 0) { //解密
                perror("decry err");
                return DES_DECRY_ERR;
            }

            cout << "Receive message from <" << ntohl(clientAddr.sin_addr.s_addr) << ">: "
                << decryResult << endl;

            cin.getline(sMsg, sizeof(sMsg)); // 不用cin，因为不能含空格
            if(strcmp(sMsg, "quit") == 0) {
                break;
            }
            
            cout << "Send message to <" << ntohl(clientAddr.sin_addr.s_addr) << ">: " 
                << sMsg << endl;

            string encryResult; // 加密结果
            if (des.Encry(sMsg, DES_KEY, encryResult) != 0) { // 加密
                perror("encry err");
                return DES_ENCRY_ERR;
            }
            memset(sMsg, '\0', MSG_SIZE);
            for (int i = 0; i < encryResult.length(); i++) { // 加密结果string转char[]
                sMsg[i] = encryResult[i];
            }
            sMsg[encryResult.size()] = '\0';
            
            if(send(fd_client, sMsg, strlen(sMsg), 0) <= 0) { // （6）send将服务器的消息发给客户端
                perror("server send err");
                return SERVER_SEND_ERR;
            }
            // recv和send的返回值：
            // >0 表示成功，返回实际发送或接受的字节数
            // =0 表示超时，对方主动关闭了连接过程
            // <0 出错
        }
        close(fd_client);
        
        cout << "当前客户端已结束通信，是否继续等待其他客户端？（1 - 是， 0 - 否）" << endl;
        bool isContinue = false;
        cin >> isContinue;
        if (!isContinue) {
           break;
        }
    }
    
    cout << "-- server end --" << endl;
    close(fd_skt);
    return SUCCESS;
}

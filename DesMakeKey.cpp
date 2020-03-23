#include <iostream>
#include <string.h>
#include <vector>
#include "Des.h"
#include "DefineCode.h"
#include "HelpFunc.h"
using namespace std;

// 生成16个密钥中的每一个子密钥
vector< vector<bool> > CDesOperate::MakeKey(vector<bool> initKey) {
    // (0) 密钥的奇偶校验位
    for (int i = 0; i < 8; i++) {
        int count1 = 0;
        for (int j = 0; j < 7; j++) {
            if (initKey[i * 8 + j] == true) {
                count1++;
            }
        }
        if (count1 % 2 == 0) { //当前为偶数个1，本组最后一个数字改为1
            initKey[i * 8 + 7] = true;
        } // 否则改为0
        else {
            initKey[i * 8 + 7] = false;
        }
    }

    // (1) 置换选择PC-1：从64bit初始密钥中选出56bit有效位
    // 64 = 8位奇偶校验 + 左28 + 右28, int = 4字节 = 4*8位
    // 初始密钥的第8、16、24、32、40、48、56、64位是奇偶校验位，其他为有效位
    vector< vector<bool> > keyPC1(2);
    for (int i = 0; i < 28; i++) {
        keyPC1[0].push_back(initKey[keyLeft[i] - 1]);
        keyPC1[1].push_back(initKey[keyRight[i] - 1]);
    }

    //（2）循环左移LS
    // keyPC1[0]和keyPC1[1]各自左移特定位，共16轮，每轮左移位数不同
    vector< vector<bool> > keyLS; // 1维是16轮，2维是每一轮的28*2=56位子秘钥
    for (int i = 0; i < 16; i++) {
        keyPC1[0] = leftShift(leftTable[i], keyPC1[0]);
        keyPC1[1] = leftShift(leftTable[i], keyPC1[1]);
        vector<bool> tempKeyLS;
        tempKeyLS.insert(tempKeyLS.end(), keyPC1[0].begin(), keyPC1[0].end());
        tempKeyLS.insert(tempKeyLS.end(), keyPC1[1].begin(), keyPC1[1].end());
        keyLS.push_back(tempKeyLS);
    }

    // （3）置换选择PC2
    // 将其余位置按照keyChoose置换位置，输出48bit，作为第N轮的子密钥
    vector< vector<bool> > keyPC2; // 1维是16轮，2维是每一轮的48位子秘钥
    for (int i = 0; i < 16; i++) {
        vector<bool> tempKeyPC2;
        for (int j = 0; j < 48; j++) {
            tempKeyPC2.push_back(keyLS[i][keyChoose[j] - 1]);
        }
        keyPC2.push_back(tempKeyPC2);
    }

    return keyPC2;
}
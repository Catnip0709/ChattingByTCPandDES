#include <iostream>
#include <vector>
#include "Des.h"
#include "HelpFunc.h"
using namespace std;

// f函数 = 选择扩展运算E + 密钥加运算 + 选择压缩运算S + 置换P
vector<bool> CDesOperate::fFunc(vector<bool> input, vector<bool> key) {
    vector<bool> fStep1 = EBox(input);
    vector<bool> fStep2 = keyAddition(fStep1, key);
    vector<bool> fStep3 = selectCompressionOperation(fStep2);
    vector<bool> fStep4 = replacementOp(fStep3);
    return fStep4;
}

//（1）选择扩展运算(E盒)：将输入的右边32bit扩展成为48bit输出
vector<bool> CDesOperate::EBox(vector<bool> input) {
    vector<bool> output;
    for (int i = 0; i < 48; i++) {
        output.push_back(input[des_E[i] - 1]);
    }
    return output;
}

//（2）秘钥加运算：将选择扩展运算输出的48bit作为输入，与48bit的子密钥进行异或运算^
vector<bool> CDesOperate::keyAddition(vector<bool> input, vector<bool> key) {
    return XOR(input, key);
}

//（3）选择压缩运算
vector<bool> CDesOperate::selectCompressionOperation(vector<bool> input) {
    // 1、将密钥加运算的输出作为48bit输入，将其分为8组，每组6bit，
    vector<vector<bool> > inputGroup;
    for (int i = 0; i < 8; i++) {
        vector<bool> tempGroup;
        for (int j = 0; j < 6; j++) {
            tempGroup.push_back(input[i * 6 + j]);
        }
        inputGroup.push_back(tempGroup);
    }

    vector<bool> result;
    // 2、分别进入8个S盒进行运算，得出8*4=32bit的输出结果，拼接后输出
    for (int i = 0; i < 8; i++) {
        int tempInt = des_S[i][from2To10(inputGroup[i])];
        vector<bool> tempBool = from10To2(tempInt, 4);
        result.insert(result.end(), tempBool.begin(), tempBool.end());
    }
    return result;
}

//（4）置换运算P
vector<bool> CDesOperate::replacementOp(vector<bool> input) {
    vector<bool> result;
    for (int i = 0; i < 32; i++) {
        result.push_back(input[des_P[i] - 1]);
    }
    return result;
}
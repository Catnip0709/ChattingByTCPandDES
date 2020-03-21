#include <iostream>
#include <vector>
#include <map>
#include "Des.h"
#include "DefineCode.h"
#include "HelpFunc.h"
using namespace std;

int CDesOperate:: Encry(string plainText, string key, string& encryResult) {
    if(key.size() != 8) {
        cout << "err: key size isn't 8" << endl;
        return DES_ERR_BIT;
    }
    vector<bool> binKey = KeyProcess(key);
    vector<vector<bool>> binPlainText = DataProcess(plainText);
    vector<vector<bool>> binCipherText; //二进制结果
    for (int i = 0; i < binPlainText.size(); i++) { // 分组处理
        vector<bool> tempResult = EncryPro(binPlainText[i], binKey);
        binCipherText.push_back(tempResult);
    }

    //将2进制的加密结果转为string
    encryResult = "";
    for(int i = 0; i < binCipherText.size(); i++) {
        encryResult += from2To10(binCipherText[i]);
    }
    return SUCCESS;
}

vector<bool> CDesOperate:: EncryPro(vector<bool> input, vector<bool> key) {
    // 生成子秘钥
    vector<vector<bool>> subKey = MakeKey(key);

    // 步骤1: 初始置换IP
    vector<bool> tempStep1 = InitReplacementIP(input, INIT_REPLACE_IP); 
    map<string, vector<bool>> step1;
    vector<bool> tempLeft, tempRight;
    for(int i = 0; i < 32; i++) {
        tempLeft.push_back(tempStep1[i]);
        tempRight.push_back(tempStep1[i + 32]);
    }
    step1["left"] = tempLeft;
    step1["right"] = tempRight;

    // 步骤2: 16轮迭代
    // 每一轮：左 = 右， 右 = f(右, subkey)^左 
    for(int i = 0; i < 16; i++) {
        vector<bool> fFuncResult = fFunc(step1["right"], subKey[i]);
        vector<bool> xorResult = XOR(fFuncResult, step1["left"]);

        step1["left"]  = step1["right"];
        step1["right"] = xorResult;
    }

    // 步骤3: 逆初始置换IP
    vector<bool> step3 = step1["left"];
    step3.insert(step3.end(), step1["right"].begin(), step1["right"].end());
    step3 = InitReplacementIP(step3, INVERSE_REPLACE_IP); 

    return step3;
}
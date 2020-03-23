#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Des.h"
#include "DefineCode.h"
#include "HelpFunc.h"
using namespace std;

int CDesOperate::Decry(string cipherText, string key, string& decryResult) {
    if (key.size() != 8) {
        cout << "err: key size isn't 8" << endl;
        return DES_ERR_BIT;
    }

    vector<bool> binKey = KeyProcess(key); // key预处理，string转vector<bool>
    vector< vector<bool> > subKey = MakeKey(binKey); // 生成子秘钥

    vector< vector<bool> > binCipherText = DecryDataProcess(cipherText); 
    vector< vector<bool> > binPlainText; //明文二进制结果

    for (int i = 0; i < binCipherText.size(); i++) { // 分组处理
        vector<bool> tempResult = DecryPro(binCipherText[i], subKey);
        binPlainText.push_back(tempResult);
    }
    
    // 2进制结果分组，64 = 8 * 8
    for (int i = 0; i < binPlainText.size(); i++) {
        string tempResult = "";
        int start = 0;
        while (start < 64) {
            vector<bool> cuttedBin(binPlainText[i].begin() + start, binPlainText[i].begin() + start + 8);
            tempResult += from2To10(cuttedBin);
            start += 8;
        }
        decryResult += tempResult;
    }
    
    return SUCCESS;
}

vector<bool> CDesOperate:: DecryPro(vector<bool> input, vector<vector<bool> > subKey) {
    // 步骤1: 初始置换IP
    vector<bool> tempStep1 = InitReplacementIP(input, INIT_REPLACE_IP);
    map<string, vector<bool> > step1;
    vector<bool> tempLeft, tempRight;
    for (int i = 0; i < 32; i++) {
        tempLeft.push_back(tempStep1[i]);
        tempRight.push_back(tempStep1[i + 32]);
    }

    step1["left"] = tempLeft;
    step1["right"] = tempRight;

    // 步骤2: 16轮迭代
    // 每一轮：右 = 左，左 = f(左, subkey)^右 
    for (int i = 0; i < 16; i++) {
        vector<bool> fFuncResult = fFunc(step1["left"], subKey[15 - i]);
        vector<bool> xorResult = XOR(fFuncResult, step1["right"]);

        step1["right"] = step1["left"];
        step1["left"] = xorResult;
    }

    // 步骤3: 逆初始置换IP
    vector<bool> step3 = step1["left"];
    step3.insert(step3.end(), step1["right"].begin(), step1["right"].end());
    step3 = InitReplacementIP(step3, INVERSE_REPLACE_IP);
    return step3;
}
#include <iostream>
#include <vector>
#include <map>
#include "Des.h"
#include "DefineCode.h"
#include "HelpFunc.h"
using namespace std;

// 初始置换IP，逆初始置换IP
vector<bool> CDesOperate:: InitReplacementIP(vector<bool> input, int type) {
    int *pc_table = NULL;
    if (type == INIT_REPLACE_IP) {
        pc_table = pc_first;
    }
    else if (type == INVERSE_REPLACE_IP) {
        pc_table = pc_last;
    }

    vector<bool> result;
    for(int i = 0; i < 64; i++) {
        result.push_back(input[pc_table[i] - 1]);
    }

    return result;
}

// 数据处理
vector<vector<bool>> CDesOperate:: DataProcess(string text) {
    vector<string> cuttedText; // 8个字母为一组的切割
    int groupNum = text.size()/8;
    for (int i = 0; i < groupNum; i++) { // 按组压入
        string tempText = "";
        for (int j = 0; j < 8; j++) {
            tempText.push_back(text[8 * i + j]);
        }
        cuttedText.push_back(tempText);
    }
    if (text.size() % 8 != 0) { // 末尾有小于8个字符
        string tempText = "";
        for (int i = groupNum * 8; i < text.size(); i++) {
            tempText.push_back(text[i]);
        }
        // 将最后一组string补全为8个字符 
        for(int i = tempText.size(); i < 8; i++) {
            tempText.push_back('\0');
        }
        cuttedText.push_back(tempText);
    }

    // 将每一组的8个字符转为8*8=64bit的二进制
    vector<vector<bool>> binText;
    for(int i = 0; i < cuttedText.size(); i++) {
        vector<bool> tempBin;
        for(int j = 0; j < cuttedText[i].size(); j++){
            vector<bool> tempBinPart = from10To2(cuttedText[i][j], 8);
            tempBin.insert(tempBin.end(), tempBinPart.begin(), tempBinPart.end());
        }
        binText.push_back(tempBin);
    }
    return binText;
}

// key的处理
vector<bool> CDesOperate:: KeyProcess(string key) {
    vector<bool> result;
    for(int i = 0; i < key.size(); i++) {
        vector<bool> tempResult = from10To2(key[i], 8);
        result.insert(result.end(), tempResult.begin(), tempResult.end());
    }
    return result;
}
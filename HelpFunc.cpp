#include <iostream>
#include <vector>
#include "DefineCode.h"
#include "HelpFunc.h"
using namespace std;

// 循环左移
vector<bool> leftShift(int shiftBit, vector<bool> LSVector) {
    shiftBit = shiftBit % LSVector.size();
    vector<bool> result;
    for (int i = 0; i < LSVector.size() - shiftBit; i++) {
        result.push_back(LSVector[i + shiftBit]);
    }
    for (int i = 0; i < shiftBit; i++) {
        result.push_back(LSVector[i]);
    }
    return result;
}

// 二进制转十进制
int from2To10(vector<bool> binary) {
    int result = 0, help = 1;
    for (int i = binary.size() - 1; i >= 0; i--) {
        int temp = binary[i] == 1 ? help : 0;
        result += temp;
        help = help << 1;
    }
    return result;
}

// 十进制转二进制
vector<bool> from10To2(int decimal, int bit) { // 参数2：将十进制decimal转成bit位二进制
    int help = 1;
    vector<bool> result;
    for (int i = 0; i < bit; i++) {
        if (decimal & help) {
            result.insert(result.begin(), true);
        }
        else {
            result.insert(result.begin(), false);
        }
        help = help << 1;
    }
    return result;
}

// 异或运算^
vector<bool> XOR(vector<bool> input1, vector<bool> input2) {
    vector<bool> output;
    for (int i = 0; i < input1.size(); i++) {
        output.push_back(input1[i] ^ input2[i]);
    }
    return output;
}
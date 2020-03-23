#include <iostream>
#include <vector>
#include "DefineCode.h"
using namespace std;

// 循环左移
vector<bool> leftShift(int shiftBit, vector<bool> LSVector);

// 二进制转十进制
int from2To10(vector<bool> binary);

// 十进制转二进制
vector<bool> from10To2(int decimal, int bit);

// 异或运算^
vector<bool> XOR(vector<bool> input1, vector<bool> input2);

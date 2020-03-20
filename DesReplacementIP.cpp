#include <iostream>
#include <vector>
#include "DES.h"
#include "DefineCode.h"
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

    // 将 64 bit 的明文重新排列，而后分成左右两块，每块 32bit，用L0和R0表示
    vector<bool> result;
    for (int i = 0; i < 64; i++) {
        result.push_back(input[pc_table[i] - 1]);
    }
    return result;
}
#pragma once

#include "iostream"

#include "common/global.hpp"

static bool init = false;

struct code_info {

    void assign(uint32_t value, uint8_t len) {
        code_info::value = value;
        code_info::len = len;
    };

    uint32_t value;
    uint8_t len;
};

extern code_info ExpCode[511];

void ExpCodeInit();

void ExpCodeGen();

inline uint32_t getExpCode(int value) {
    if (!init) ExpCodeInit();
    return ExpCode[(255 + value) % 511].value;
}

inline code_info getExpCodeInfo(int value) {
    if (!init) ExpCodeInit();
    return ExpCode[(255 + value) % 511];
}

std::string getExpCodeString(int value);
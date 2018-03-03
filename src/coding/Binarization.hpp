#pragma once

#include "iostream"

#include "common/global.hpp"
#include <bitset>
//#include "coding/BitStream/BitStream.hpp"

class BitStream;
static bool init = false;

struct code_info {

    code_info(){}

    code_info(uint32_t value, uint8_t len) : value(value), len(len) {}

    void assign(uint32_t value, uint8_t len) {
        code_info::value = value;
        code_info::len = len;
    };

    uint32_t value;
    uint8_t len;

    std::string toString() {
        return std::bitset<32>(value).to_string().substr(0, len);
    }
};

/************************************************
 *             Exponential Golomb Codes k=0
 ************************************************/
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


int getExpDecode(BitStream &bitStream);

std::string getExpCodeString(int value);

/************************************************
 *             Code for DC cofficients
 ************************************************/

code_info getDCCodeInfo(int value);

int       getDCDecode(BitStream &bitStream);
#pragma once

#include "iostream"

#include "common/global.hpp"

static bool init = false;

static uint32_t ExpCode[511];

void ExpCodeInit();

void ExpCodeGen();

inline uint32_t getExpCode(int value) {
    if (!init) ExpCodeInit();
    return ExpCode[(255 + value) % 511];
}

std::string getExpCodeString(int value);
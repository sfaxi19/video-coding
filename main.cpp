#include <iostream>
#include <cmath>
#include <bitset>
#include <sys/time.h>
#include <chrono>
#include "AVIMaker/AVIMaker.h"
#include "compression/Coder.hpp"
#include "compression/ExpGolombCodes.hpp"
#include "compression/CabacFsm.hpp"
#include "compression/common/global.hpp"

int main() {
    //system("rm log.txt");
    AVIMaker avi_file("../avi-maker/resources/lr1_1.AVI");
    avi_file.saveVideoStreamToBMP("cm_files/");
    avi_to_h264(avi_file);
    //ExpCodeGen();
    //uint32_t id1 = timer_start();

    //static_cast<uint8_t>((((double)rand()/RAND_MAX) < 0.8) ? 1 : 0)
/*
    std::string data = "000000000000000000000011010000000000100000000000000000001100000000000000000000001010";

    uint8_t  MPS = 0;
    uint16_t stateIdx = 60;
    CabacFsm cabac(MPS, stateIdx);
    for (int i = 0; i < data.size(); i++) {
        cabac.encodingEngine(getBit(data[i]));
    }
    cabac.terminateEncode(1);
    CabacFsm cabacDecode(MPS, stateIdx, cabac.getEncode());
    for (int i = 0; i < data.size(); i++) {
        cabacDecode.decodingEngine();
    }
    //cabacDecode.terminateDecode();

    LOG(INFO, "Input data:  %s", data.c_str());
    LOG(INFO, "Encode data: %s", cabac.getEncode().c_str());
    if (data == cabacDecode.getDecode()) {
        LOG(INFO, ANSI_COLOR_GREEN "Decode data: %s", cabacDecode.getDecode().c_str());
    } else {
        LOG(INFO, ANSI_COLOR_RED "Decode data: %s", cabacDecode.getDecode().c_str());
    }*/
    //LOG(ERROR, "Fail!");
    //system("rm log.txt");
    return 0;
}
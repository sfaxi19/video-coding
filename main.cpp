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
    //AVIMaker avi_file("../avi-maker/resources/lr1_1.AVI");
    //avi_file.saveVideoStreamToBMP("cm_files/");
    //avi_to_h264(avi_file);
    //ExpCodeGen();
    //uint32_t id1 = timer_start();

    //static_cast<uint8_t>((((double)rand()/RAND_MAX) < 0.8) ? 1 : 0)

    CabacFsm cabac(0);
    //srand(time(nullptr));
    std::string data = "111111111111101011101101110111111011111101111111011101111111111010111111110000";

    for (int i = 0; i < data.size(); i++) {
        cabac.encodingEngine(static_cast<uint8_t>((data[i] == '1') ? 1 : 0));
    }
    CabacFsm cabacDecode(0, cabac.getEncode());
    for (int i = 0; i < data.size(); i++) {
        cabacDecode.decodingEngine();
    }
    //timer_stop_us(id1);
    LOG(INFO, "Input data:  %s", data.c_str());
    LOG(INFO, "Encode data: %s", cabacDecode.getEncode().c_str());
    LOG(INFO, "Decode data: %s", cabacDecode.getDecode().c_str());
    system("rm log.txt");
    return 0;
}
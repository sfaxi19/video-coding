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
#include "compression/BitStream.hpp"

int main() {
    //system("rm log.txt");
    AVIMaker avi_file("../avi-maker/resources/lr1_1.AVI");
    avi_file.saveVideoStreamToBMP("cm_files/");
    //avi_to_h264(avi_file);
    //ExpCodeGen();
    BitStream bitStream;
    bitStream.bushBit(1);
    bitStream.bushBit(0);
    bitStream.bushBit(1);
    bitStream.bushBit(1);
    bitStream.bushBit(0);
    bitStream.bushBit(1);
    bitStream.bushBit(0);
    bitStream.bushBit(1);
    bitStream.bushBit(1);
    bitStream.bushBit(0);
    LOG(INFO, "%s", bitStream.toString().c_str());

    return 0;
}
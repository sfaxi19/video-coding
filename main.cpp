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
    CabacFsm cabac(1);
    for (int i = 0; i < 20; i++) {
        cabac.encodeBin(1);
    }
    CabacFsm cabacDecode(1);
    cabacDecode.
    //timer_stop_us(id1);
    system("rm log.txt");
    return 0;
}
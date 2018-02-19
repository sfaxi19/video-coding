#include <iostream>
#include <cmath>
#include <bitset>
#include <sys/time.h>
#include <chrono>
#include "AVIMaker/AVIMaker.h"
#include "coding/Coder.hpp"
#include "coding/ExpGolombCodes.hpp"
#include "coding/CabacFsm.hpp"
#include "coding/BitStream.hpp"
#include "common/global.hpp"

int main() {
    //system("rm log.txt");
    AVIMaker avi_file("../avi-maker/resources/lr1_1.AVI");
    avi_file.saveVideoStreamToBMP("cm_files/");
    //avi_to_h264(avi_file);
    //ExpCodeGen();
    BitStream bitStream;
    //bitStream.pushBits("1001001010101010010010100100101001");
    code_info code = getExpCodeInfo(5);
    bitStream.pushBits(code.value, code.len);
    code = getExpCodeInfo(20);
    bitStream.pushBits(code.value, code.len);
    //LOG(INFO, "ExpCode[5]=%s", getExpCodeString(5).c_str());
    LOG(INFO, "%s", bitStream.toString().c_str());

    return 0;
}
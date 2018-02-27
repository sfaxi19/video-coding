#include <iostream>
#include <cmath>
#include <bitset>
#include <sys/time.h>
#include <chrono>
#include <bmp_lib/bmp.h>
#include "AVIMaker/AVIMaker.h"
#include "coding/Coder.hpp"
#include "coding/ExpGolombCodes.hpp"
#include "coding/CabacFsm.hpp"
#include "coding/BitStream.hpp"
#include "common/global.hpp"
#include "Blocks.hpp"

int main() {
    //system("rm log.txt");
    AVIMaker avi_file("../avi-maker/resources/lr1_3.avi");
    print_bitmap_info(avi_file.video()->bmInfo());
    print_avi_header(avi_file.aviHeader);
    StreamHeader streamHeader = avi_file.videoStreams.at(0)->streamHeader();
    print_stream_header(streamHeader);
    avi_to_h264(avi_file);
    avi_file.saveVideoStreamToBMP("cm_files/");

    //ExpCodeGen();

    //Block4x4Layout block4x4(avi_file.video()->getFrame(1), 100, 100);
    //LOG(INFO, "\n%s", block4x4.toString().c_str());

    /*
    BitStream bitStream;
    //bitStream.pushBits("1001001010101010010010100100101001");
    code_info code = getExpCodeInfo(5);
    bitStream.pushBits(code.value, code.len);
    code = getExpCodeInfo(20);
    bitStream.pushBits(code.value, code.len);

    uint8_t MPS = 1;
    uint16_t stateIdx = 63;
    CabacFsm cabac(MPS, stateIdx);
    std::string data = "11111111110";
    for (int i = 0; i < data.size(); i++) {
        cabac.encodingEngine(static_cast<uint8_t>((data[i] == '1') ? 1 : 0));
    }
    cabac.terminateEncode(1);
    std::string enc = cabac.getEncode();

    CabacFsm cabac_dec(MPS, stateIdx,enc);
    for (int i = 0; i < data.size(); i++) {
        cabac_dec.decodingEngine();
    }


    LOG(INFO, "Enc: %s", enc.c_str());
    LOG(INFO, "Dec: %s", cabac_dec.getDecode().c_str());
    LOG(INFO, "Inp: %s", data.c_str());
    //LOG(INFO, "ExpCode[5]=%s", getExpCodeString(5).c_str());
    LOG(INFO, "%s", bitStream.toString().c_str());
*/
    return 0;
}
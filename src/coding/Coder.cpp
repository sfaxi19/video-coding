//
// Created by sfaxi19 on 28.12.17.
//
#include <AVIMaker/AVIMaker.h>
#include "Coder.hpp"
#include "bmp_lib/bmp.h"
#include "motion_compensation/motion_compensation.h"
#include "Scanning.hpp"
#include "common/global.hpp"
#include "ExpGolombCodes.hpp"
#include "DCT.hpp"
#include "CabacFsm.hpp"
#include "Quantization.hpp"

inline int sign(int x) {
    return (x >= 0) ? 1 : -1;
}

void mark_range(TRIPLERGB **frame, int x, int y, int h, int w) {
    for (int i = y; i < y + h; i++)
        frame[i][x].red = 255;
    for (int i = y; i < y + h; i++)
        frame[i][x + w - 1].red = 255;
    for (int i = x; i < x + w; i++)
        frame[y][i].red = 255;
    for (int i = x; i < x + w; i++)
        frame[y + h - 1][i].red = 255;
}

void print_block(double block[][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_block(const char *title, double **block) {
    printf("%s\n", title);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

std::string block_to_string(double **block) {
    std::string binStr = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
    }
    printf("\n");
}

void print_block(TRIPLEYCbCr **frame, int x, int y, int h, int w) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            printf("%8d", frame[i][j].Y);
        }
        printf("\n");
    }
    printf("\n");
}


void block_from_frame(double **block, TRIPLEYCbCr **frame, mc::block_info block_info) {
    for (int i = 0; i < 4; i++) {
        block[i] = new double[4];
        for (int j = 0; j < 4; j++) {
            block[i][j] = frame[block_info.y + i][block_info.x + j].Y;
        }
    }
}

void
subtract_to_4x4_block(TRIPLEYCbCr **base, TRIPLEYCbCr **target, double **block, mc::block_info block_info, mc::vect v) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = target[block_info.y + i][block_info.x + j].Y -
                          base[block_info.y + v.y + i][block_info.x + v.x + j].Y;
        }
    }
}

uint8_t getBit(char ch) {
    return static_cast<uint8_t>((ch == '1') ? 1 : 0);
}

void getFreq(int *freq, double **block) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j == 0)) continue;
            freq[256 + (int) block[i][j]]++;
        }
    }
}

double blockEntropy(const int freq[], int len, int N) {
    double sum = 0;
    double p = 0;
    for (int i = 0; i < len; i++) {
        p = ((double) freq[i] / N);
        if (p == 0) continue;
        sum += p * log2(p);
    }
    return sum * (-1);
}

void avi_to_h264(AVIMaker &aviMaker) {
    VideoStream *video = aviMaker.videoStreams[0];//video();
    TRIPLEYCbCr **frame1 = RGB2YCbCr(video->getFrame(7), video->height(), video->width());
    TRIPLEYCbCr **frame2 = RGB2YCbCr(video->getFrame(8), video->height(), video->width());
    TRIPLEYCbCr **out = new TRIPLEYCbCr *[video->height()];
    for (int i = 0; i < video->height(); i++) {
        out[i] = new TRIPLEYCbCr[video->width()];
    }
    int posX = 120;
    int posY = 120;
    double **block = new double *[4];
    for (int i = 0; i < 4; i++) {
        block[i] = new double[4];
    }
    mc::block_info block_info(posX, posY, 4, 4);
    mc::vect v(1, 1);

    print_block(frame1, posX, posY, 4, 4);
    print_block(frame2, posX + v.x, posY + v.y, 4, 4);
    //subtract_block(frame1, frame2, m_enc, block_info, v);
    //subtract_to_4x4_block(frame1, frame1, block, block_info, v);
    block_from_frame(block, frame1, block_info);

    print_block("BLOCK", block);
    int dc = dct(block);
    print_block("DCT:", block);
    int QS = 9;
    quant_block(block, QS);
    print_block("QUANT:", block);

    std::string data = "";

    Scanning scan(Scanning::ZIG_ZAG);
    while (!scan.stop()) {
        mc::pos pos = scan.nextStep();
        data.append(getExpCodeString((uint32_t) block[pos.y][pos.x]));
        LOG(INFO, "%3d - %s", (int) block[pos.y][pos.x], getExpCodeString((uint32_t) block[pos.y][pos.x]).c_str());
    }

    int freq[512] = {0};
    getFreq(freq, block);
    LOG(INFO, "H = %f", blockEntropy(freq, 512, 15) * 15);
    for (uint8_t m = 0; m < 2; m++) {
        for (uint16_t i = 0; i < 63; i++) {
            uint8_t MPS = m;
            uint16_t stateIdx = i;
            CabacFsm cabac(MPS, stateIdx);
            for (int i = 0; i < data.size(); i++) {
                cabac.encodingEngine(getBit(data[i]));
            }
            cabac.terminateEncode(1);
            LOG(INFO, "MPS: %d,\tstateIdx: %d\tSize: %lu", MPS, stateIdx, cabac.getEncode().size());
        }
    }
    //
    /*
    CabacFsm cabacDecode(MPS, stateIdx, cabac.getEncode());
    for (int i = 0; i < data.size(); i++) {
        cabacDecode.decodingEngine();
    }
    */
    //cabacDecode.terminateDecode();


    LOG(INFO, "Input data:  %s\t(%lu bits)", data.c_str(), data.size());
    //LOG(INFO, "Encode data: %s\t(%lu bits)", cabac.getEncode().c_str(), cabac.getEncode().size());
    /*if (data == cabacDecode.getDecode()) {
        LOG(INFO, ANSI_COLOR_GREEN
                "Decode data: %s", cabacDecode.getDecode().c_str());
    } else {
        LOG(INFO, ANSI_COLOR_RED
                "Decode data: %s", cabacDecode.getDecode().c_str());
    }*/

    iquant_block(block, QS);
    print_block("IQUANT:", block);


    idct(block);
    print_block("IDCT:", block);

}

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

double Qstep[10] = {0.625, 0.6875, 0.8125, 0.875, 1, 1.125, 1.25, 1.375, 1.625, 1.75};

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

int w[6][3] = {{13107, 5243, 8066},
               {11916, 4660, 7490},
               {10082, 4194, 6554},
               {9362,  3647, 5825},
               {8192,  3355, 5243},
               {7282,  2893, 4559}};
int v[6][3] = {{10, 16, 13},
               {11, 18, 14},
               {13, 20, 16},
               {14, 23, 18},
               {16, 25, 20},
               {18, 29, 23}};

int LevelScale(int m, int i, int j) {
    if (((i == 0) && (j == 0)) ||
        ((i == 2) && (j == 0)) ||
        ((i == 0) && (j == 2)) ||
        ((i == 2) && (j == 2))) {
        return v[m][0];
    }
    if (((i == 1) && (j == 1)) ||
        ((i == 1) && (j == 3)) ||
        ((i == 3) && (j == 1)) ||
        ((i == 3) && (j == 3))) {
        return v[m][1];
    }
    return v[m][2];
}

int A(int i, int j) {
    if (((i == 0) && (j == 0)) ||
        ((i == 2) && (j == 0)) ||
        ((i == 0) && (j == 2)) ||
        ((i == 2) && (j == 2))) {
        return 16;
    }
    if (((i == 1) && (j == 1)) ||
        ((i == 1) && (j == 3)) ||
        ((i == 3) && (j == 1)) ||
        ((i == 3) && (j == 3))) {
        return 25;
    }
    return 20;
}

int LevelScale2(int m, int i, int j) {
    if (((i == 0) && (j == 0)) ||
        ((i == 2) && (j == 0)) ||
        ((i == 0) && (j == 2)) ||
        ((i == 2) && (j == 2))) {
        return w[m][0];
    }
    if (((i == 1) && (j == 1)) ||
        ((i == 1) && (j == 3)) ||
        ((i == 3) && (j == 1)) ||
        ((i == 3) && (j == 3))) {
        return w[m][1];
    }
    return w[m][2];
}

void quant_block(double **block, int QS) {
    int f = (1 << (14 + (int) floor((double) QS / 6)));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //        block[i][j] = sign(block[i][j]) * ((abs(block[i][j]) * LevelScale2(QS % 6, i, j) + f) >> (15 + (int) floor((double)QS / 6)));

            block[i][j] = round(block[i][j] / Qstep[QS]);
        }
    }
    //block[0][0] = 0;
}

void iquant_block(double **block, int QS) {
    int f = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //block[i][j] = (((int) block[i][j] * LevelScale(QS % 6, i, j) * A(i, j)) >> ((int) floor(QS / 6))) >> 10;
            block[i][j] = round(block[i][j] * Qstep[QS]);
        }
    }
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


    uint8_t  MPS = 1;
    uint16_t stateIdx = 0;
    CabacFsm cabac(MPS, stateIdx);
    for (int i = 0; i < data.size(); i++) {
        cabac.encodingEngine(getBit(data[i]));
    }
   // cabac.terminateEncode(1);
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
    }



    iquant_block(block, QS);
    print_block("IQUANT:", block);


    idct(block);
    print_block("IDCT:", block);

}

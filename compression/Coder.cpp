//
// Created by sfaxi19 on 28.12.17.
//
#include <AVIMaker/AVIMaker.h>
#include "Coder.hpp"
#include "bmp_lib/bmp.h"
#include "motion_compensation/motion_compensation.h"

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

const double a = (double) 1 / 2;
const double a2 = pow(a, 2);
const double b = sqrt((double) 2 / 5);
const double b2 = pow(b, 2);
const double ab_2 = (a * b) / 2;
double E[4][4] = {
        {a2,   ab_2,   a2,   ab_2},
        {ab_2, b2 / 4, ab_2, b2 / 4},
        {a2,   ab_2,   a2,   ab_2},
        {ab_2, b2 / 4, ab_2, b2 / 4}
};
double C[4][4] = {
        {1, 1,  1,  1},
        {2, 1,  -1, -2},
        {1, -1, -1, 1},
        {1, -2, 2,  -1}};

double CT[4][4] = {
        {1, 2,  1,  1},
        {1, 1,  -1, -2},
        {1, -1, -1, 2},
        {1, -2, 1,  -1}};
double Ci[4][4] = {
        {1,   1,   1,    1},
        {1,   0.5, -0.5, -1},
        {1,   -1,  -1,   1},
        {0.5, -1,  1,    -0.5}};

double CTi[4][4] = {
        {1, 1,    1,  0.5},
        {1, 0.5,  -1, -1},
        {1, -0.5, -1, 1},
        {1, -1,   1,  -0.5}};
double Ei[4][4] = {
        {a2,    a * b, a2,    a * b},
        {a * b, b2,    a * b, b2},
        {a2,    a * b, a2,    a * b},
        {a * b, b2,    a * b, b2}
};

double Qstep[10] = {0.625, 0.6875, 0.8125, 0.875, 1, 1.125, 1.25, 1.375, 1.625, 1.75};
enum class MultipMode : int {
    NORM = 0,
    REVERSE = 1,
    SIMPLE
};

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

void print_block(TRIPLEYCbCr **frame, int x, int y, int h, int w) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            printf("%8d", frame[i][j].Y);
        }
        printf("\n");
    }
    printf("\n");
}

void multiple(double **block, double matrix[][4], double **out, MultipMode mode) {
    double mrx[4][4];
    if (mode != MultipMode::SIMPLE) {
        for (int id_h = 0; id_h < 4; id_h++) {
            for (int id_w = 0; id_w < 4; id_w++) {
                mrx[id_h][id_w] = 0;
                for (int i = 0; i < 4; i++) {
                    mrx[id_h][id_w] += (mode == MultipMode::NORM) ?
                                       block[id_h][i] * matrix[i][id_w] :
                                       matrix[id_h][i] * block[i][id_w];
                }
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (mode != MultipMode::SIMPLE)out[i][j] = mrx[i][j];
            else out[i][j] = block[i][j] * matrix[i][j];
        }
    }
}

void idct(double **block) {
    multiple(block, E, block, MultipMode::SIMPLE);
    multiple(block, CT, block, MultipMode::REVERSE);
    multiple(block, C, block, MultipMode::NORM);
}

int dct(double **block) {
    multiple(block, C, block, MultipMode::REVERSE);
    multiple(block, CT, block, MultipMode::NORM);
    multiple(block, E, block, MultipMode::SIMPLE);
    return (int) block[0][0];
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
    //subtract_block(frame1, frame2, out, block_info, v);
    //subtract_to_4x4_block(frame1, frame1, block, block_info, v);
    block_from_frame(block, frame1, block_info);

    print_block("BLOCK", block);
    int dc = dct(block);
    print_block("DCT:", block);
    int QS = 9;
    quant_block(block, QS);
    print_block("QUANT:", block);

    iquant_block(block, QS);
    //block[0][0] = dc;
    print_block("IQUANT:", block);
    //multiple(block, E, block, MultipMode::SIMPLE);
    //multiple(block, E, block, MultipMode::SIMPLE);

    idct(block);
    print_block("IDCT:", block);
    /* printf("mrx1:\n");
     print_block(frame1, posX, posY, 4, 4);
     printf("mrx2:\n");
     print_block(frame2, posX + v.x, posY + v.y, 4, 4);
     printf("Diff:\n");
     print_block(out, posX, posY, 4, 4);

 */
    //mark_range((TRIPLERGB **) frame1, posX, posY, 4, 4);
    //save_component_to_files((TRIPLEBYTES **) frame1, video->bmFile(), video->bmInfo(), COMPONENT_Y,
    //                        "currentframe.bmp");

    //dct(block);
}



/*
 *
DCT:
  314.50   -5.69    0.50   -1.26
    2.37    0.20    0.47    0.10
   -1.00    1.26   -1.00    2.21
   -0.79   -1.40   -0.16    4.30

QUANT:
  125.00   -1.00    0.00    0.00
    0.00    0.00    0.00    0.00
    0.00    0.00    0.00    0.00
    0.00    0.00    0.00    0.00

IQUANT:
  312.00   -5.00    0.00    0.00
    0.00    0.00    0.00    0.00
    0.00    0.00    0.00    0.00
    0.00    0.00    0.00    0.00
 *
 */
//
// Created by sfaxi19 on 18.02.18.
//
#include "cmath"
#include "Quantization.hpp"

//0     1       2       3       4       5
double Qstep[52] = {0.625, 0.6875, 0.8125, 0.875, 1, 1.125,
                    1.25, 1.375, 1.625, 1.75, 2, 2.25,
                    2.5, 2.75, 3.25, 3.5, 4, 4.5,
                    5, 5.5, 6.5, 7, 8, 9,
                    10, 11, 13, 14, 16, 18,
                    20, 22, 26, 28, 32, 36,
                    40, 44, 52, 56, 64, 72,
                    80, 88, 104, 112, 128, 144,
                    160, 176, 208, 224};


const int w[6][3] = {{13107, 5243, 8066},
                     {11916, 4660, 7490},
                     {10082, 4194, 6554},
                     {9362,  3647, 5825},
                     {8192,  3355, 5243},
                     {7282,  2893, 4559}};

const int v[6][3] = {{10, 16, 13},
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

/*
 * Returns quantity of not zero elements
 */
int quant_block(int **block, int QS) {
    int num = 0;
    int f = (1 << (14 + (int) floor((double) QS / 6)));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //        block[i][j] = sign(block[i][j]) * ((abs(block[i][j]) * LevelScale2(QS % 6, i, j) + f) >> (15 + (int) floor((double)QS / 6)));
            double qstep = Qstep[QS];
            block[i][j] = static_cast<int>(round(block[i][j] / qstep));
            num += (block[i][j] == 0) ? 0 : 1;
        }
    }
    return num;
    //block[0][0] = 0;
}

void iquant_block(int **block, int QS) {
    int f = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //block[i][j] = (((int) block[i][j] * LevelScale(QS % 6, i, j) * A(i, j)) >> ((int) floor(QS / 6))) >> 10;
            double qstep = Qstep[QS];
            block[i][j] = static_cast<int>(round(block[i][j] * qstep));
        }
    }
}
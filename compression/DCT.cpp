//
// Created by sfaxi19 on 17.02.18.
//
#include "DCT.hpp"

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
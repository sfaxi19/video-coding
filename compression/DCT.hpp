//
// Created by sfaxi19 on 17.02.18.
//

#ifndef VIDEO_COMPRESSION_DCT_HPP
#define VIDEO_COMPRESSION_DCT_HPP
#include "cmath"

enum class MultipMode : int {
    NORM    = 0,
    REVERSE = 1,
    SIMPLE  = 2
};

void multiple(double **block, double matrix[][4], double **out, MultipMode mode);

void idct(double **block);

int dct(double **block);

#endif //VIDEO_COMPRESSION_DCT_HPP

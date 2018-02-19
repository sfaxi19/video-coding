#ifndef DCT_HPP
#define DCT_HPP

#include "cmath"

enum class MultipMode : int {
    NORM    = 0,
    REVERSE = 1,
    SIMPLE  = 2
};

void multiple(double **block, double matrix[][4], double **out, MultipMode mode);

void idct(double **block);

int dct(double **block);

void idct_dc(double **block);

int dct_dc(double **block);

#endif //DCT_HPP

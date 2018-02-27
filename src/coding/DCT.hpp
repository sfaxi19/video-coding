#ifndef DCT_HPP
#define DCT_HPP

#include <Blocks.hpp>
#include "cmath"

class IBlock4x4;
class Block4x4_float;
class Block4x4Layout;

//void multiple(double **block, double matrix[][4], double **out, MultipMode mode);

void idct(double **block);

int dct(double **block);

void dct(IBlock4x4& block);

void dct(Block4x4_float &block);

void idct_dc(double **block);

int dct_dc(double **block);

#endif //DCT_HPP

#ifndef DCT_HPP
#define DCT_HPP

#include <Blocks.hpp>
#include "cmath"

class IBlock4x4;
class Block4x4_float;
class Block4x4Layout;

//void multiple(double **block, double matrix[][4], double **out, MultipMode mode);

void idct(int **block);

int dct(int **block);

void dct(IBlock4x4& block);

void dct(Block4x4_float &block);

void idct_dc(int **block);

int dct_dc(int **block);

#endif //DCT_HPP

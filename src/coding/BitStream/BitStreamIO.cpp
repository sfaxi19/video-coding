//
// Created by sfaxi19 on 03.03.18.
//
#include "BitStreamIO.hpp"

/******************************************************************************
 * ============================================================================
 *                               Block_info
 * ============================================================================
 * +------+--------+
 * | EG0  |  Type  |
 * +------+--------+
 * | Type |   4x4  |
 * +------+--------+
 *
 * Types:
 * 0 - EG0
 * 1 - UEG0 + cabac
 * 2 - zeros block
 *
 * ============================================================================
 *                              Macroblock_info
 * ============================================================================
 *
 * +------------------------+
 * |  if (frame_type == P)  |
 * +------------+-----------+----------+-------+-----------------------+
 * |     EG0    |    EG0    | dc_level |  EG0  |       Block_info      |
 * +------------+-----------+----------+-------+-----------------------+
 * |  mb_type   |    MVD    | DC diff  | DC4x4 | 4x4 | 4x4 | ... | 4x4 |
 * +------------+-----------+----------+-------+-----+-----+-----+-----+
 *
 * mb_type:
 * 0 - I
 * 1 - P
 *
 * ============================================================================
 *                                Frame_info
 * ============================================================================
 *
 * +---------------+------------------------------------------+
 * |     EG0       |             Macroblock_info              |
 * +---------------+---------+---------+--------+-----+-------+
 * |  frame_type   |   MB1   |   MB2   |   MB3  | ... |  MBn  |
 * +---------------+---------+---------+--------+-----+-------+
 *
 * frame_type:
 * 0 - I
 * 1 - P
 * ***************************************************************************/

void MacroblockInfoToBitStream(macroblock_info &mb_res, BitStream &bitStream) {
    /*****************************
     *      push diff DC level
     ****************************/
    code_info code = getDCCodeInfo(mb_res.dc_level);
    bitStream.pushBits(code);
    /*****************************
     *      push DC4x4 block
     ****************************/
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j == 0)) continue;
            code = getExpCodeInfo(static_cast<int>(mb_res.dc_block[i][j]));
            bitStream.pushBits(code);
        }
    }
    /*****************************
     *   push residual4x4 blocks
     ****************************/
    for (int iBlkIdx = 0; iBlkIdx < 4; iBlkIdx++) {
        for (int jBlkIdx = 0; jBlkIdx < 4; jBlkIdx++) {
            double **block = mb_res.block[iBlkIdx][jBlkIdx];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if ((i == 0) && (j == 0)) continue;
                    bitStream.pushBits(getExpCodeInfo(static_cast<int>(block[i][j])));
                }
            }
        }
    }
}

void BitStreamToMacroblockInfo(macroblock_info &mb_res, BitStream &bitStream) {
    /*****************************
    *      get diff DC level
    ****************************/
    mb_res.dc_level = getDCDecode(bitStream);
    /*****************************
     *      get DC4x4 block
     ****************************/
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j == 0)) continue;
            mb_res.dc_block[i][j] = getExpDecode(bitStream);
        }
    }
    mb_res.dc_block[0][0] = mb_res.dc_level;
    /****************************
     *   get residual4x4 blocks
     ****************************/
    for (int h = 0; h < 4; h++) {
        for (int w = 0; w < 4; w++) {
            double **block = mb_res.block[h][w];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if ((i == 0) && (j == 0)) continue;
                    block[i][j] = getExpDecode(bitStream);
                    //bitStream.pushBits(getExpCodeInfo(static_cast<int>(abs(block[i][j]))));
                }
            }
        }
    }
}
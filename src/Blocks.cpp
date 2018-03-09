//
// Created by sfaxi19 on 20.02.18.
//
#include <bmp_lib/bmp.h>
#include "Blocks.hpp"

void print_block(const char *title, int **block) {
    printf("%s\n", title);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8d", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void MacroblockInfo::ioBitStream(BitStream &bitStream, int mode) {
    //print();
    /*****************************
     *       diff DC level
     *****************************/
    if (mode == MODE_CODE) {
        bitStream.pushBits(getDCCodeInfo(dc_level));
    } else {
        dc_level = getDCDecode(bitStream);
    }
    //code_info code = getDCCodeInfo(mb_res->dc_level);
    //bitStream.pushBits(code);
    /*****************************
     *        DC4x4 block
     *****************************/
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j == 0)) continue;
            if (mode == MODE_CODE) {
                bitStream.pushBits(getDCCodeInfo(dc_block[i][j]));
            } else {
                dc_block[i][j] = getDCDecode(bitStream);
            }
        }
    }
    dc_block[0][0] = 0;
    /*****************************
     *    residual4x4 blocks
     *****************************/
    for (int iBlkIdx = 0; iBlkIdx < 4; iBlkIdx++) {
        for (int jBlkIdx = 0; jBlkIdx < 4; jBlkIdx++) {
            int **pToBlock = block[iBlkIdx][jBlkIdx];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if ((i == 0) && (j == 0)) continue;
                    if (mode == MODE_CODE) {
                        bitStream.pushBits(getExpCodeInfo(pToBlock[i][j]));
                    } else {
                        pToBlock[i][j] = getExpDecode(bitStream);
                    }
                }
            }
            pToBlock[0][0] = 0;
        }
    }
}

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

void FrameInfo::ioBitStream(BitStream &bitStream, int mode) {
    /**************************
     *       frame_type:
     *         0 - I
     *         1 - B
     **************************/
    if (mode == MODE_CODE) {
        bitStream.pushBits(getExpCodeInfo(frame_type));
    } else {
        frame_type = getExpDecode(bitStream);
    }
    /**************************
     *       frame_data
     **************************/
    if (mode == MODE_DECODE) {
        int len = 3 * ((h * w) / 256);
        macroblocks.resize(len);
        for (int i = 0; i < len; i++) {
            if (!macroblocks[i])macroblocks[i] = new MacroblockInfo;
        }
    }
    for (auto mb : macroblocks) {
        mb->ioBitStream(bitStream, mode);
    }
}

void FrameInfo::saveFrame(BITMAPFILEHEADER bmFile, BITMAPINFOHEADER bmInfo, std::string filename) {
    TRIPLEYCbCr **frame = new TRIPLEYCbCr *[h];
    for (int i = 0; i < h; i++) {
        frame[i] = new TRIPLEYCbCr[w];
    }
    int mbIdx = 0;
    for (int component = COMPONENT_Y; component <= COMPONENT_CR; component++) {
        for (int i = 0; i < ceil(h / 16); i++) {
            for (int j = 0; j < ceil(w / 16); j++) {
                mc::block_info block16x16pos(j * 16, i * 16, 16, 16);
                save_to_frame(frame, macroblocks[mbIdx], block16x16pos, component);
                mbIdx++;
            }
        }
    }
    //save_component_to_files((TRIPLEBYTES **) frame, bmFile, bmInfo, COMPONENT_Y, filename.c_str());
    YCbCr2RGB((TRIPLEBYTES **) frame, h, w);
    saveBMPFile(bmFile, bmInfo, (TRIPLERGB **) frame, filename.c_str());
}

void
FrameInfo::save_to_frame(TRIPLEYCbCr **frame, MacroblockInfo *mb_info, mc::block_info block16x16pos, int component) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int **pToBlock = mb_info->block[i][j];
            for (int c = 0; c < 4; c++) {
                for (int d = 0; d < 4; d++) {
                    setComponent((TRIPLEBYTES **) frame,
                                 block16x16pos.y + i * 4 + c,
                                 block16x16pos.x + j * 4 + d,
                                 clip(pToBlock[c][d], 0, 255), component);
                }
            }
        }
    }
}

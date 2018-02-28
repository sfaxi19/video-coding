//
// Created by sfaxi19 on 28.12.17.
//
#include <AVIMaker/AVIMaker.h>
#include <Blocks.hpp>
#include "Coder.hpp"
#include "bmp_lib/bmp.h"
#include "motion_compensation/motion_compensation.h"
#include "Scanning.hpp"
#include "common/global.hpp"
#include "Binarization.hpp"
#include "DCT.hpp"
#include "CabacFsm.hpp"
#include "Quantization.hpp"

inline int sign(int x) {
    return (x >= 0) ? 1 : -1;
}


void mark_range(TRIPLERGB **frame, int x, int y, int h, int w, int component) {
    for (int i = y; i < y + h; i++)
        setComponent((TRIPLEBYTES **) frame, i, x, 255, component);//frame[i][x].red = 255;
    for (int i = y; i < y + h; i++)
        setComponent((TRIPLEBYTES **) frame, i, x + w - 1, 255, component); //frame[i][x + w - 1].red = 255;
    for (int i = x; i < x + w; i++)
        setComponent((TRIPLEBYTES **) frame, y, i, 255, component); //frame[y][i].red = 255;
    for (int i = x; i < x + w; i++)
        setComponent((TRIPLEBYTES **) frame, y + h - 1, i, 255, component); //frame[y + h - 1][i].red = 255;
}


void mark_range(TRIPLERGB **frame, mc::block_info bi, int component) {
    mark_range(frame, (int) bi.x, (int) bi.y, (int) bi.height, (int) bi.width, component);
}

void print_block(double block[][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


void print_block2x2(const char *title, double **block) {
    printf("%s\n", title);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%8.2f", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

std::string block_to_string(double **block) {
    std::string binStr = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
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


void print_block(TRIPLEYCbCr **frame, mc::block_info bi) {
    print_block(frame, (int) bi.x, (int) bi.y, (int) bi.height, (int) bi.width);
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
subtract_to_16x16_block(TRIPLEYCbCr **base, TRIPLEYCbCr **target, double **block, mc::block_info block_info,
                        mc::vect v) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = target[block_info.y + i][block_info.x + j].Y -
                          base[block_info.y + v.y + i][block_info.x + v.x + j].Y;
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

void
addition_to_4x4_block(TRIPLEYCbCr **base, double **block, mc::block_info block_info, mc::vect v) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = block[i][j] + base[block_info.y + v.y + i][block_info.x + v.x + j].Y;
        }
    }
}

uint8_t getBit(char ch) {
    return static_cast<uint8_t>((ch == '1') ? 1 : 0);
}

void getFreq(int *freq, double **block) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j == 0)) continue;
            freq[256 + (int) block[i][j]]++;
        }
    }
}

double blockEntropy(const int freq[], int len, int N) {
    double sum = 0;
    double p = 0;
    for (int i = 0; i < len; i++) {
        p = ((double) freq[i] / N);
        if (p == 0) continue;
        sum += p * log2(p);
    }
    return sum * (-1);
}

void BitStreamToMacroblockResult(macroblock_result &mb_res, BitStream &bitStream) {
    mb_res.dc_level = getDCDecode(bitStream);
    /*****************************
     *      push DC4x4 block
     ****************************/
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //code = getExpCodeInfo(static_cast<int>(abs(mb_res.dc_block[i][j])));
            //bitStream.pushBits(code);
        }
    }
}

/******************************************************************************
 * +----------+-------+-----------------------+
 * | dc_code  |  EG0  |          EG0          |
 * +----------+-------+-----------------------+
 * | DC diff  | DC4x4 | 4x4 | 4x4 | 4x4 | 4x4 |
 * +----------+-------+-----+-----+-----+-----+
 * ***************************************************************************/
void MacroblockResultToBitStream(macroblock_result &mb_res, BitStream &bitStream) {
    //mb_res.print();
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
            code = getExpCodeInfo(static_cast<int>(abs(mb_res.dc_block[i][j])));
            bitStream.pushBits(code);
        }
    }

    LOG(INFO, "------------------------");
    /*****************************
     *   push residual4x4 blocks
     ****************************/
    for (int h = 0; h < 4; h++) {
        for (int w = 0; w < 4; w++) {
            double **block = mb_res.block[h][w];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    bitStream.pushBits(getExpCodeInfo(static_cast<int>(abs(block[i][j]))));
                }
            }
        }
    }
}

int macroblock_coding(TRIPLEYCbCr **frame, mc::block_info &block16x16info, macroblock_result &mb_res) {
    LOG(MAIN, "%s(frame=%p, x=%lu, y=%lu, w=%lu h=%lu)", __FUNCTION__, frame,
        block16x16info.x, block16x16info.y,
        block16x16info.width, block16x16info.height);
    int QS = 9;
    size_t block_size = 4;
    size_t blocks_in_width = block16x16info.width / block_size;
    size_t blocks_in_height = block16x16info.height / block_size;
    /*double **block = new double *[block_size];
    double **dc_block = new double *[block_size];
    for (int i = 0; i < block_size; i++) {
        block[i] = new double[4];
        dc_block[i] = new double[4];
    }*/
    double **dc_block = mb_res.dc_block;
    for (size_t i = 0; i < blocks_in_height; i++) {
        for (size_t j = 0; j < blocks_in_width; j++) {
            double **block = mb_res.block[i][j];
            mc::block_info block4x4info(block16x16info.x + j * block_size, block16x16info.y + i * block_size, 4, 4);
            block_from_frame(block, frame, block4x4info);
            //print_block("Block:", block);
            /****************************************************
             *                       DCT
             ****************************************************/
            dc_block[i][j] = dct(block);
            //print_block("DCT:", block);
            /****************************************************
             *                   Quantization
             ****************************************************/
            quant_block(block, QS);
            //print_block("QUANT:", block);
        }
    }
    //print_block("DC_BLOCK:", dc_block);
    int dc = dct(dc_block);

    quant_block(dc_block, QS);

    //print_block("DC_QUANT:", dc_block);

/*
    iquant_block(dc_block, QS);
    print_block("DC_IQUANT:", dc_block);

    idct(dc_block);
    print_block("DC_BLOCK:", dc_block);
*/
    return dc;
}

void frame_coding(TRIPLEYCbCr **base, TRIPLEYCbCr **target, size_t defWidth, size_t defHeight, BitStream &bitStream) {
    LOG(MAIN, "%s(base=%p, target=%p, w=%lu, h=%lu)", __FUNCTION__, base, target, defWidth, defHeight);
    size_t block_size = 16;
    size_t blocks_in_width = defWidth / block_size;
    size_t blocks_in_height = defHeight / block_size;
    bool first_flag = true;
    int dc_prev = 0;
    macroblock_result mb_res;

    for (size_t i = 0; i < blocks_in_height; i++) {
        for (size_t j = 0; j < blocks_in_width; j++) {
            mc::block_info block16x16info(j * block_size, i * block_size, block_size, block_size);
            //mark_range(video->getFrame(8), blockInfo, COMPONENT_A);
            if (base) {
                mc::vect v = mc::logarithmicSearch(base, target, 20, 20, block16x16info);
                mc::subtract_block(base, target, target, block16x16info, v);
            }
            int dc = macroblock_coding(target, block16x16info, mb_res);

            if (first_flag) {
                mb_res.dc_level = dc;
                first_flag = false;
            } else {
                mb_res.dc_level = dc - dc_prev;
            }

            MacroblockResultToBitStream(mb_res, bitStream);
            dc_prev = dc;
            return;
        }
    }
    //mc::block_info blockInfo1(100, 100, 4, 4);
    //save_component_to_files((TRIPLEBYTES **) target, video->bmFile(), video->bmInfo(), COMPONENT_A, "out.bmp");
}


void avi_to_h264(AVIMaker &aviMaker) {
    VideoStream *video = aviMaker.videoStreams[0]; //video();
    TRIPLEYCbCr **base = RGB2YCbCr(video->getFrame(7), video->height(), video->width());
    TRIPLEYCbCr **target = RGB2YCbCr(video->getFrame(8), video->height(), video->width());
    const size_t defHeight = video->height();
    const size_t defWidth = video->width();
    BitStream bitStream;
    frame_coding(nullptr, target, defWidth, defHeight, bitStream);
    LOG(INFO, "RESULT: %s", bitStream.toString().c_str());
    return;
    //TRIPLEYCbCr **out = new TRIPLEYCbCr *[video->height()];
    //for (int i = 0; i < video->height(); i++) {
    //    out[i] = new TRIPLEYCbCr[video->width()];
    //}

    //print_block(frame1, posX, posY, 4, 4);
    //print_block(frame2, posX + v.x, posY + v.y, 4, 4);
    //subtract_block(frame1, frame2, m_enc, block_info, v);
    //subtract_to_4x4_block(frame1, frame1, block, block_info, v);
    size_t block_size = 16;
    size_t blocks_in_width = defWidth / block_size;
    size_t blocks_in_height = defHeight / block_size;
    for (size_t i = 0; i < blocks_in_height; i++) {
        for (size_t j = 0; j < blocks_in_width; j++) {
            mc::block_info blockInfo(j * block_size, i * block_size, block_size, block_size);
            mark_range(video->getFrame(8), blockInfo, COMPONENT_A);
            mc::vect v = mc::logarithmicSearch(base, target, 20, 20, blockInfo);
            //mc::subtract_block(base, target, target, blockInfo, v);
        }
    }
    //save_component_to_files((TRIPLEBYTES **) target, video->bmFile(), video->bmInfo(), COMPONENT_A, "out.bmp");
    mc::block_info blockInfo1(100, 100, 4, 4);
    mc::block_info blockInfo2(120, 124, 4, 4);
    mc::block_info blockInfo3(124, 120, 4, 4);
    mc::block_info blockInfo4(124, 124, 4, 4);
    double **block = new double *[4];
    for (int i = 0; i < 4; i++) {
        block[i] = new double[4];
    }

    double **dc = new double *[2];
    for (int i = 0; i < 2; i++) {
        dc[i] = new double[2];
    }


    /****************************************************
     *                  Read block
     ****************************************************/
    //block_from_frame(block, frame1, blockInfo1);
    mc::vect v = mc::logarithmicSearch(base, target, 20, 20, blockInfo1);
    subtract_to_4x4_block(base, target, block, blockInfo1, v);
    print_block("BLOCK1:", block);
    /****************************************************
     *                     DCT
     ****************************************************/
    dc[0][0] = dct(block);
    print_block("DCT:", block);
    /****************************************************
     *                   Quantization
     ****************************************************/
    int QS = 9;
    quant_block(block, QS);
    print_block("QUANT:", block);
    /****************************************************
     *                  Check Entropy
     ****************************************************/
    int freq[512] = {0};
    getFreq(freq, block);
    LOG(INFO, "H = %f", blockEntropy(freq, 512, 15) * 15);
    /****************************************************
     *                  IQuantization
     ****************************************************/
    iquant_block(block, QS);
    print_block("IQUANT:", block);
    /****************************************************
     *                      IDCT
     ****************************************************/
    idct(block);
    print_block("IDCT:", block);

    /****************************************************
     *                  Addition block
     ****************************************************/
    addition_to_4x4_block(base, block, blockInfo1, v);
    print_block(target, blockInfo1);
    print_block("Result:", block);
    /*
    block_from_frame(block, frame1, blockInfo2);
    print_block("BLOCK2:", block);
    dc[0][1] = dct(block);
    block_from_frame(block, frame1, blockInfo3);
    print_block("BLOCK3:", block);
    dc[1][0] = dct(block);
    block_from_frame(block, frame1, blockInfo4);
    print_block("BLOCK4:", block);
    dc[1][1] = dct(block);
*/
    print_block2x2("DC:", dc);
    /*LOG(INFO, "dc1=%f", dc);
    dc = dc1 - dc2;
    LOG(INFO, "dc2=%f", dc);
    dc = dc2 - dc3;
    LOG(INFO, "dc3=%f", dc);
    dc = dc3 - dc4;
    LOG(INFO, "dc4=%f", dc);*/
    //print_block("BLOCK", block);
    //print_block("DCT:", block);
    //Block4x4Layout block4x4Layout(frame1, 120, 120);

    //LOG(INFO, "\n%s", block4x4.toString("BLOCK:").c_str());

    /*
    uint32_t id2 = timer_start();
    for(int i=0; i<1000000; i++) {
        Block4x4_float block4x4(block4x4Layout);
        dct(block4x4);
    }
    timer_stop_us(id2);
    */
    //LOG(INFO, "\n%s", block4x4Layout.toString().c_str());



    //LOG(INFO, "\n%s", block4x4.toString("DCT:").c_str());


    /*int QS = 9;

    quant_block(block, QS);
    print_block("QUANT:", block);

    std::string data = "";

    Scanning scan(Scanning::ZIG_ZAG);
    while (!scan.stop()) {
        mc::pos pos = scan.nextStep();
        data.append(getExpCodeString((uint32_t) block[pos.y][pos.x]));
        LOG(INFO, "%3d - %s", (int) block[pos.y][pos.x], getExpCodeString((uint32_t) block[pos.y][pos.x]).c_str());
    }

    int freq[512] = {0};
    getFreq(freq, block);
    LOG(INFO, "H = %f", blockEntropy(freq, 512, 15) * 15);
    for (uint8_t m = 0; m < 2; m++) {
        for (uint16_t i = 0; i < 63; i++) {
            uint8_t MPS = m;
            uint16_t stateIdx = i;
            CabacFsm cabac(MPS, stateIdx);
            for (int i = 0; i < data.size(); i++) {
                cabac.encodingEngine(getBit(data[i]));
            }
            cabac.terminateEncode(1);
            LOG(INFO, "MPS: %d,\tstateIdx: %d\tSize: %lu", MPS, stateIdx, cabac.getEncode().size());
        }
    }
    //

    //CabacFsm cabacDecode(MPS, stateIdx, cabac.getEncode());
    //for (int i = 0; i < data.size(); i++) {
    //    cabacDecode.decodingEngine();
    //}
    //
    //cabacDecode.terminateDecode();


    LOG(INFO, "Input data:  %s\t(%lu bits)", data.c_str(), data.size());
    */
    //LOG(INFO, "Encode data: %s\t(%lu bits)", cabac.getEncode().c_str(), cabac.getEncode().size());
    /*if (data == cabacDecode.getDecode()) {
        LOG(INFO, ANSI_COLOR_GREEN
                "Decode data: %s", cabacDecode.getDecode().c_str());
    } else {
        LOG(INFO, ANSI_COLOR_RED
                "Decode data: %s", cabacDecode.getDecode().c_str());
    }*/

    /*
    iquant_block(block, QS);
    print_block("IQUANT:", block);


    idct(block);
    print_block("IDCT:", block);
*/
}

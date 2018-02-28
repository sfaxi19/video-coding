//
// Created by sfaxi19 on 20.02.18.
//
//
// Created by sfaxi19 on 20.02.18.
//

#include <bmp_lib/bmp_headers.hpp>
#include <bmp_lib/bmp.h>
//#include <coding/DCT.hpp>
#include "iostream"
#include "Blocks.hpp"

/*
 * for (int id_w = 0; id_w < 4; id_w++) {
                mrx[id_h][id_w] = 0;
                for (int i = 0; i < 4; i++) {
                    mrx[id_h][id_w] += (mode == MultipMode::NORM) ?
                                       block[id_h][i] * matrix[i][id_w] :
                                       matrix[id_h][i] * block[i][id_w];
                }
            }
 */

Block4x4_float block4x4_float;

Block4x4_float Block4x4_float::multiple(const Block4x4_float &matrix, MultipMode mode) {
    if (mode != MultipMode::SIMPLE) {
        for (uint16_t id_h = 0; id_h < 4; id_h++) {
            for (uint16_t id_w = 0; id_w < 4; id_w++) {
                double value = 0;
                for (uint16_t i = 0; i < 4; i++) {
                    value += (mode == MultipMode::NORM) ?
                             m_block[id_h][i] * matrix.getValue(id_w, i) :
                             matrix.getValue(i, id_h) * m_block[i][id_w];
                }
                block4x4_float.setValue(value, id_w, id_h);
            }
        }
    }
    for (uint16_t i = 0; i < 4; i++) {
        for (uint16_t j = 0; j < 4; j++) {
            if (mode == MultipMode::SIMPLE) {
                setValue(getValue(j, i) * matrix.getValue(j, i), j, i);
            } else {
                setValue(block4x4_float.getValue(j, i), j, i);
            }
        }
    }

}

std::string Block4x4_float::toString(const std::string &title) {
    std::string str = title + "\n";
    char formstr[32];
    for (uint16_t i = 0; i < 4; i++) {
        for (uint16_t j = 0; j < 4; j++) {
            sprintf(formstr, "%8.2f", getValue(j, i));
            str.append(formstr);
        }
        str.append("\n");
    }
    return str;
}



void print_block(const char *title, double **block) {
    printf("%s\n", title);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.2f", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
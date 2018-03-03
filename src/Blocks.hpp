//
// Created by sfaxi19 on 20.02.18.
//
#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#include <bmp_lib/bmp_headers.hpp>
#include <bmp_lib/bmp.h>
#include <coding/DCT.hpp>
#include <common/global.hpp>
#include <motion_compensation/motion_compensation.h>
#include "iostream"

enum class MultipMode : int {
    NORM = 0,
    REVERSE = 1,
    SIMPLE = 2
};
enum mbType : int {
    I = 0,
    P = 1
};

void print_block(const char *title, double **block);


struct macroblock_info {

    macroblock_info() {
        dc_block = new double *[4];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                block[i][j] = new double *[4];
                for (int b = 0; b < 4; b++) {
                    block[i][j][b] = new double[4];
                }
            }
            dc_block[i] = new double[4];
        }
    }

    virtual ~macroblock_info() {
        //LOG(WARN, "Macroblock_info destructor!");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int b = 0; b < 4; b++) {
                    delete[] block[i][j][b];
                }
                delete[] block[i][j];
            }
            delete[] dc_block[i];
        }
        delete[] dc_block;
    }

    void print() {
        printf("DC Level: %d\n", dc_level);
        print_block("DC_BLOCK:", dc_block);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                print_block("BLOCK:", block[i][j]);
            }
        }
    }

    // 'block_pattern' consists of
    // / 1 - if block non zeros
    // \ 0 - otherwise
    int block_pattern[4][4]{};
    double **block[4][4]{};
    double **dc_block;
    int dc_level = 0;
    uint8_t mb_type = 0;
    mc::vect v{};
};

/*
 *

 class IBlock4x4 {

protected:

    //const uint16_t m_h = 4;
    //const uint16_t m_w = 4;
    //bool isFloat = false;

public:
    //IBlock4x4() {};

    //virtual void setValue(double, uint16_t x, uint16_t y) = 0;

    //virtual void setValue(uint8_t value, uint16_t x, uint16_t y) = 0;

    //virtual double getValue(uint16_t x, uint16_t y) const = 0;

//Block4x4_float multiple(IBlock4x4 &block, const IBlock4x4 &matrix, MultipMode mode);

//std::string toString(const std::string &title = "");
};

class Block4x4Layout {
private:
    uint16_t m_x = 0;
    uint16_t m_y = 0;
    TRIPLEBYTES **m_frame = nullptr;
    int m_component = -1;

public:

    Block4x4Layout(TRIPLERGB **frame, uint16_t x, uint16_t y, int component = COMPONENT_A) : Block4x4Layout(
            (TRIPLEBYTES **) frame, x, y, component) {}

    Block4x4Layout(TRIPLEYCbCr **frame, uint16_t x, uint16_t y, int component = COMPONENT_A) : Block4x4Layout(
            (TRIPLEBYTES **) frame, x, y, component) {}

    Block4x4Layout(TRIPLEBYTES **frame, uint16_t x, uint16_t y, int component) {
        m_x = x;
        m_y = y;
        m_frame = frame;
        m_component = component;
    }

    void setValue(uint8_t value, uint16_t x, uint16_t y) {
        //if ((x < 4) && (y < 4)) {
        setComponent(m_frame, m_y + y, m_x + x, value, m_component);
        //}
    }

    void setValue(double value, uint16_t x, uint16_t y) {
        setValue(static_cast<uint8_t>(value), x, y);
    }

    double getValue(uint16_t x, uint16_t y) const {
        //if ((x >= 4) || (y >= 4)) return 0;
        return getComponent(m_frame, m_y + y, m_x + x, m_component);
    }
};

class Block4x4_float {
private:
    double m_block[4][4];
public:

    Block4x4_float() {}

    Block4x4_float(const Block4x4_float &block) {
        //LOG(MAIN, "%s. Констуктор копирования1", __FUNCTION__);
        for (uint16_t i = 0; i < 4; i++) {
            for (uint16_t j = 0; j < 4; j++) {
                setValue(block.getValue(j, i), j, i);
            }
        }
    }

    Block4x4_float(Block4x4Layout &block) {
        //LOG(MAIN, "%s. Констуктор копирования2", __FUNCTION__);
        for (uint16_t i = 0; i < 4; i++) {
            for (uint16_t j = 0; j < 4; j++) {
                setValue(block.getValue(j, i), j, i);
            }
        }
    }

    Block4x4_float(double block[4][4]) {
        for (uint16_t i = 0; i < 4; i++) {
            for (uint16_t j = 0; j < 4; j++) {
                m_block[i][j] = block[i][j];
            }
        }
    }

    inline void setValue(double value, uint16_t x, uint16_t y) {
        //if ((x < 4) && (y < 4)) {
        m_block[y][x] = value;
        //}
    }


    inline double getValue(uint16_t x, uint16_t y) const {
        //if ((x >= 4) || (y >= 4)) return 0;
        return m_block[y][x];
    }

    Block4x4_float multiple(const Block4x4_float &matrix, MultipMode mode);

    std::string toString(const std::string &title = "");
};
*/

#endif //BLOCKS_HPP

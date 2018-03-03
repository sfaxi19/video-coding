//
// Created by sfaxi19 on 18.02.18.
//

#ifndef VIDEO_COMPRESSION_BITSTREAM_HPP
#define VIDEO_COMPRESSION_BITSTREAM_HPP

#include "iostream"
#include "coding/Binarization.hpp"
#include <vector>

struct code_info;

class BitStream {
private:
    std::vector<uint8_t> m_bitStream;
    int m_bit_pos = -1;
    int m_read_bit_pos = 0;
    int m_byte_pos = -1;
    size_t m_bit_length = 0;
    size_t m_byte_length = 0;

    uint8_t *getCurrentByte();

    uint8_t *getByte(int byte_pos);

    void newByte();

public:
    BitStream();

    /*********************************
     *    Add values to BitStream
     *********************************/
    void pushBit(uint8_t bit);

    void pushBits(std::string &bits);

    void pushBits(uint32_t dword, size_t len = 32);

    void pushBits(code_info code);

    /*********************************
     *   Read values from BitStream
     *********************************/
    uint8_t readNext();

    void readReset();

    bool isEnd();

    uint8_t getBit(int pos);

    /*********************************
     *       Other methods
     *********************************/
    std::string toString(size_t max_length = 0);
};


#endif //VIDEO_COMPRESSION_BITSTREAM_HPP

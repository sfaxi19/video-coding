//
// Created by sfaxi19 on 18.02.18.
//

#ifndef VIDEO_COMPRESSION_BITSTREAM_HPP
#define VIDEO_COMPRESSION_BITSTREAM_HPP

#include "iostream"
#include <vector>

const uint8_t shifts[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001
};

class BitStream {
private:
    std::vector<uint8_t> m_bitStream;
    int m_bit_pos = -1;
    int m_byte_pos = -1;
    size_t m_bit_length = 0;
    size_t m_byte_length = 0;

    uint8_t *getCurrentByte();

    uint8_t *getByte(int byte_pos);

    void newByte();

public:
    BitStream();

    void pushBit(uint8_t bit);

    void pushBits(std::string bits);

    void pushBits(uint32_t dword, size_t len = 32);

    uint8_t getBit(int pos);

    std::string toString();
};


#endif //VIDEO_COMPRESSION_BITSTREAM_HPP

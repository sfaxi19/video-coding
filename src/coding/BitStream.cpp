//
// Created by sfaxi19 on 18.02.18.
//

#include "BitStream.hpp"
#include "common/global.hpp"

static const uint8_t shifts[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001
};

std::string BitStream::toString() {
    std::string binString = "[";
    for (int i = 0; i < m_bit_length; i++) {
        binString.append(std::to_string(getBit(i)));
    }
    return binString.append("] \n(").append(std::to_string(m_bit_length) + " bits")
            .append(" / " + std::to_string(m_byte_length) + " bytes)");
}

void BitStream::pushBit(uint8_t value) {
    m_bit_pos++;
    //LOG(MAIN, "%s(value=%u, bit_pos=%d)", __FUNCTION__, value, m_bit_pos);
    if (m_bit_pos % 8 == 0) {
        newByte();
    }
    if (value == 1) {
        uint8_t *cur_byte = getCurrentByte();
        *cur_byte = *cur_byte | shifts[m_bit_pos % 8];
    }
    m_bit_length++;
}

uint8_t *BitStream::getByte(int byte_pos) {
    return (m_bitStream.data() + byte_pos);
}

uint8_t *BitStream::getCurrentByte() {
    return getByte(m_byte_pos);
}

void BitStream::newByte() {
    m_byte_pos++;
    //LOG(MAIN, "%s(byte_pos=%d)", __FUNCTION__, m_byte_pos);
    m_bitStream.push_back(0x00);
    m_byte_length++;
}

uint8_t BitStream::getBit(int pos) {
    if (pos >= m_bit_length) return 0;
    int byte_pos = static_cast<int>(pos / 8);
    int bit_pos = pos % 8;
    uint8_t *byte = getByte(byte_pos);
    return static_cast<uint8_t>(((*byte & shifts[bit_pos]) != 0) ? 1 : 0);
}

BitStream::BitStream() {
    LOG(MAIN, "%s()", __FUNCTION__);
}

void BitStream::pushBits(std::string &bits) {
    for (char bit : bits) {
        if (bit != '0') {
            pushBit(1);
        } else {
            pushBit(0);
        }

    }
}

void BitStream::pushBits(uint32_t dword, size_t len) {
    uint32_t shift_bit = 0x80000000;
    if (len > 32) len = 32;
    for (int i = 0; i < len; i++) {
        uint8_t bit = static_cast<uint8_t>(((dword & shift_bit) != 0) ? 1 : 0);
        pushBit(bit);
        shift_bit = shift_bit >> 1;
    }
}

void BitStream::pushBits(code_info code) {
    LOG(MAIN, "%s(value=%u, len=%d)", __FUNCTION__, code.value, code.len);
    pushBits(code.value, code.len);
}

uint8_t BitStream::readNext() {
    return getBit(m_read_bit_pos++);
}

void BitStream::readReset() {
    m_read_bit_pos = 0;
}

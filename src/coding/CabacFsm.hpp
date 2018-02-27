#pragma once

#ifndef VIDEO_COMPRESSION_CABACFSM_HPP
#define VIDEO_COMPRESSION_CABACFSM_HPP

#include <utility>

#include "iostream"

class CabacFsm {
private:
    uint16_t m_stateIdx = 0;
    uint8_t m_valMPS = 0;

    uint32_t range = 510;
    uint32_t low = 0;
    uint32_t bitsOut = 0;
    uint8_t firstBitFlag = 1;
    std::string m_enc = "";
    std::string m_dec = "";
    uint32_t offset = 0;
    bool offsetInit = false;
    int pos = 0;
public:
    CabacFsm(uint8_t valMPS, uint16_t stateIdx);
    CabacFsm(uint8_t valMPS, uint16_t stateIdx, const std::string &enc) : m_valMPS(valMPS), m_stateIdx(stateIdx) {
        m_enc.append(enc);
    }

private:
    void transIdxLPS();

    void transIdxMPS();

    void putBit(uint32_t bit);

    uint32_t read_bits(int len);

    void renormEncode();

    void renormDecode();

    void flushEncode();

    void writeBits(uint32_t value, uint32_t bits);

public:
    void setStateIdx(uint16_t m_stateIdx);

    void setMPS(uint8_t m_valMPS);

    std::string getEncode() { return m_enc; }

    std::string getDecode() { return m_dec; }

    void encodingEngine(uint8_t binVal);

    void decodingEngine();

    void terminateEncode(uint8_t binVal);

    void terminateDecode();

};


#endif //VIDEO_COMPRESSION_CABACFSM_HPP

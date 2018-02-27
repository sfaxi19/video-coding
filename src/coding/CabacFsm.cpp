#include "CabacFsm.hpp"
#include "common/global.hpp"

const int TABLE_LPS_IDX = 0;
const int TABLE_MPS_IDX = 1;

const uint8_t stateTransTable[2][64] = {{0, 0, 1, 2, 2, 4, 4, 5, 6, 7,  8,  9,  9,  11, 11, 12,
                                                13, 13, 15, 15, 16, 16, 18, 18, 19, 19, 21, 21, 22, 22, 23, 24,
                                                24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 30, 31, 32, 32, 33,
                                                33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 63},
                                        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                                                17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
                                                33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                                49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63}};
const uint8_t rangeTabLPS[64][4] = {
        {128, 176, 208, 240},
        {128, 167, 197, 227},
        {128, 158, 187, 216},
        {123, 150, 178, 205},
        {116, 142, 169, 195},
        {111, 135, 160, 185},
        {105, 128, 152, 175},
        {100, 122, 144, 166},
        {95,  116, 137, 158},
        {90,  110, 130, 150},
        {85,  104, 123, 142},
        {81,  99,  117, 135},
        {77,  94,  111, 128},
        {73,  89,  105, 122},
        {69,  85,  100, 116},
        {66,  80,  95,  110},
        {62,  76,  90,  104},
        {59,  72,  86,  99},
        {56,  69,  81,  94},
        {53,  65,  77,  89},
        {51,  62,  73,  85},
        {48,  59,  69,  80},
        {46,  56,  66,  76},
        {43,  53,  63,  72},
        {41,  50,  59,  69},
        {39,  48,  56,  65},
        {37,  45,  54,  62},
        {35,  43,  51,  59},
        {33,  41,  48,  56},
        {32,  39,  46,  53},
        {30,  37,  43,  50},
        {29,  35,  41,  48},
        {27,  33,  39,  45},
        {26,  31,  37,  43},
        {24,  30,  35,  41},
        {23,  28,  33,  39},
        {22,  27,  32,  37},
        {21,  26,  30,  35},
        {20,  24,  29,  33},
        {19,  23,  27,  31},
        {18,  22,  26,  30},
        {17,  21,  25,  28},
        {16,  20,  23,  27},
        {15,  19,  22,  25},
        {14,  18,  21,  24},
        {14,  17,  20,  23},
        {13,  16,  19,  22},
        {12,  15,  18,  21},
        {12,  14,  17,  20},
        {11,  14,  16,  19},
        {11,  13,  15,  18},
        {10,  12,  15,  17},
        {10,  12,  14,  16},
        {9,   11,  13,  15},
        {9,   11,  12,  14},
        {8,   10,  12,  14},
        {8,   9,   11,  13},
        {7,   9,   11,  12},
        {7,   9,   10,  12},
        {7,   8,   10,  11},
        {6,   8,   9,   11},
        {6,   7,   9,   10},
        {6,   7,   8,   9},
        {2,   2,   2,   2}
};

void CabacFsm::transIdxLPS() { m_stateIdx = stateTransTable[TABLE_LPS_IDX][m_stateIdx]; }

void CabacFsm::transIdxMPS() { m_stateIdx = stateTransTable[TABLE_MPS_IDX][m_stateIdx]; }

void CabacFsm::encodingEngine(uint8_t binVal) {
    LOG(MAIN, "%s(binVal=%d, range=%d, low=%d, MPS=%d)", __FUNCTION__, binVal, range, low, m_valMPS);
    uint8_t rangeIdx = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][rangeIdx];
    uint32_t rangeMPS = range - rangeLPS;
    LOG(INFO, "Calculate: rangeLPS=%d\trangeMPS=%d\tlow=%d", rangeLPS, rangeMPS, low);
    if (binVal == m_valMPS) {
        LOG(INFO, "(binVal == MPS) -----> MPS");
        range = rangeMPS;
        transIdxMPS();
    } else {
        LOG(INFO, "(binVal != MPS) -----> LPS");
        range = rangeLPS;
        low = low + rangeMPS;
        LOG(INFO, "        low=%d\trange=%d", low, range);
        if (m_stateIdx == 0) {
            m_valMPS = 1 - m_valMPS;
        }
        transIdxLPS();
    }
    renormEncode();
    LOG(INFO, "========================================");
}

void CabacFsm::renormEncode() {
    LOG(MAIN, "%s(range=%d, low=%d)", __FUNCTION__, range, low);
    while (range < 256) {
        if (low < 256) {
            putBit(0);
        } else {
            if (low >= 512) {
                low = low - 512;
                putBit(1);
            } else {
                low = low - 256;
                bitsOut++;
            }
        }
        range = range << 1;
        low = low << 1;
    }
    LOG(INFO, "Output: range=%d,\tlow=%d", range, low);
}

void CabacFsm::putBit(uint32_t bit) {
    LOG(MAIN, "%s(%d).", __FUNCTION__, bit);
    if (firstBitFlag != 0) {
        firstBitFlag = 0;
    } else {
        //LOG(INFO, "%d", bit);
        m_enc.append(std::to_string(bit));
    }
    while (bitsOut > 0) {
        //LOG(INFO, "%d", 1 - bit);
        m_enc.append(std::to_string(1 - bit));
        bitsOut--;
    }

}

void CabacFsm::decodingEngine() {
    LOG(MAIN, "%s(range=%d, offset=%d, MPS=%d)", __FUNCTION__, range, offset, m_valMPS);
    if (!offsetInit) {
        offset = read_bits(9);
        offsetInit = true;
    }
    uint8_t rangeIdx = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][rangeIdx];
    uint32_t rangeMPS = range - rangeLPS;
    LOG(INFO, "Calculate: rangeLPS=%d, rangeMPS=%d", rangeLPS, rangeMPS);
    if (offset >= rangeMPS) {
        m_dec.append(std::to_string(1 - m_valMPS));
        offset = offset - rangeMPS;
        range = rangeLPS;
        if (m_stateIdx == 0) {
            m_valMPS = 1 - m_valMPS;
        }
        transIdxLPS();
    } else {
        range = rangeMPS;
        m_dec.append(std::to_string(m_valMPS));
        transIdxMPS();
    }
    renormDecode();
}

void CabacFsm::renormDecode() {
    LOG(MAIN, "%s(range=%d, offset=%d)", __FUNCTION__, range, offset);
    while (range < 256) {
        range = range << 1;
        offset = offset << 1;
        offset = offset | read_bits(1);
    }
    LOG(INFO, "Output: range=%d, offset=%d", range, offset);
}

uint32_t CabacFsm::read_bits(int len) {
    LOG(MAIN, "%s(len=%d)", __FUNCTION__, len);
    uint32_t val = 0;
    int start = pos;
    for (int i = 0; i < len; i++) {
        if ((start + i) >= m_enc.size()) return 0;
        if (m_enc[start + i] == '1') {
            val |= 1 << (len - i - 1);
        }
        pos++;
        //LOG(INFO, "%s", m_enc.c_str());
    }
    LOG(INFO, "Value: %u", val);
    return val;
}

void CabacFsm::terminateEncode(uint8_t binVal) {
    LOG(MAIN, "%s(binVal=%d)", __FUNCTION__, binVal);
    range = range - 2;
    if (binVal != 0) {
        low = low + range;
        flushEncode();
    } else {
        renormEncode();
    }
    LOG(INFO, "out from terminate.");
}

void CabacFsm::terminateDecode() {
    range = range - 2;
    if (offset >= range) {
        m_dec.append("1");
    } else {
        m_dec.append("0");
        renormDecode();
    }
}

void CabacFsm::flushEncode() {
    LOG(MAIN, "%s()", __FUNCTION__);
    range = 2;
    renormEncode();
    putBit((low >> 9) & 1);
    writeBits(((low >> 7) & 3) | 1, 2);
}

void CabacFsm::writeBits(uint32_t value, uint32_t bits) {
    LOG(MAIN, "%s(value=%d, bits=%d)", __FUNCTION__, value, bits);
    for (int i = bits - 1; i >= 0; i--) {
        m_enc.append(std::to_string(((value & (1 << i)) != 0) ? 1 : 0));
        //LOG(INFO, "i=%d", i);
    }
    LOG(INFO, "out from writeBits().");
}

CabacFsm::CabacFsm(uint8_t valMPS, uint16_t stateIdx) : m_valMPS(valMPS), m_stateIdx(stateIdx) {
    LOG(INFO, "CabacFsm init with valMPS=%d, stateIdx=%d", valMPS, stateIdx);
}

void CabacFsm::setStateIdx(uint16_t m_stateIdx) {
    CabacFsm::m_stateIdx = m_stateIdx;
}

void CabacFsm::setMPS(uint8_t m_valMPS) {
    CabacFsm::m_valMPS = m_valMPS;
}

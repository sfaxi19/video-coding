#include "CabacFsm.hpp"

void CabacFsm::encodingEngine(uint8_t binVal) {
    LOG(MAIN, "%s(%d)", __FUNCTION__, binVal);
    LOG(INFO, "bin: %d\trange: %d\tlow: %d\tvalMPS: %d", binVal, range, low, m_valMPS);
    uint8_t rangeIdx = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][rangeIdx];
    range = range - rangeLPS;
    LOG(INFO, "range_id: %d\trangeLPS: %d\trange: %d\tlow: %d", rangeIdx, rangeLPS, range, low);
    if (binVal == m_valMPS) {
        LOG(INFO, "----- MPS -----");
        transIdxMPS();
    } else {
        LOG(INFO, "----- !MPS -----");
        low = low + range;
        range = rangeLPS;
        if (m_stateIdx == 0) {
            m_valMPS = 1 - m_valMPS;
        }
        transIdxLPS();
    }
    renormEncode();
    LOG(INFO, "BitStream: %s", m_enc.c_str());
}

void CabacFsm::putBit(uint8_t bit) {
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

void CabacFsm::renormEncode() {
    LOG(MAIN, "%s(range=%d, low=%d)", __FUNCTION__, range, low);
    while (range < 256) {
        if (low < 256) {
            putBit(0);
        } else if (low >= 512) {
            low = low - 512;
            putBit(1);
        } else {
            low = low - 256;
            bitsOut++;
        }
        range = range << 1;
        low = low << 1;
    }
    LOG(INFO, "range: %d\tlow: %d", range, low);
}

void CabacFsm::decodingEngine() {
    LOG(MAIN, "%s()", __FUNCTION__);
    if (!offsetInit) {
        offset = read_bits(9);
        offsetInit = true;
    }
    LOG(INFO, "range: %d\toffset: %d\tvalMPS: %d", range, offset, m_valMPS);
    uint8_t rangeIdx = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][rangeIdx];
    range = range - rangeLPS;
    LOG(INFO, "range_id: %d\trangeLPS: %d\trange: %d\tlow: %d", rangeIdx, rangeLPS, range, low);
    if (offset >= range) {
        m_dec.append(std::to_string(1 - m_valMPS));
        offset = offset - range;
        range = rangeLPS;
        if (m_stateIdx == 0) {
            m_valMPS = 1 - m_valMPS;
        }
        transIdxLPS();
    } else {
        m_dec.append(std::to_string(m_valMPS));
        transIdxMPS();
    }
    renormDecode();
}

int pos = 0;

uint32_t CabacFsm::read_bits(int len) {
    LOG(MAIN, "%s(len=%d)", __FUNCTION__, len);
    uint32_t val = 0;
    int start = pos;
    for (int i = 0; i < len; i++) {
        if ((start + i) >= m_enc.size()) break;
        if (m_enc[start + i] == '1') {
            val |= 1 << (len - i - 1);
        }
        pos++;
        //LOG(INFO, "%s", m_enc.c_str());
    }
    LOG(INFO, "%u", val);
    return val;
}

void CabacFsm::renormDecode() {
    while (range < 256) {
        range = range << 1;
        offset = offset << 1;
        offset = offset | read_bits(1);
    }
}

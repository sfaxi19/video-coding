#include "CabacFsm.hpp"

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
    LOG(INFO, "=======================================================");
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
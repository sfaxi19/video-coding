#include "CabacFsm.hpp"

uint32_t range = 510;
uint32_t low = 0;
uint32_t bitsOut = 0;
uint8_t firstBitFlag = 1;
std::string out = "";
uint32_t offset = 0;

void CabacFsm::encodeBin(uint8_t binVal) {
    LOG(MAIN, "%s(%d)", __FUNCTION__, binVal);
    LOG(INFO, "bin: %d\trange: %d\tlow: %d\tvalMPS: %d", binVal, range, low, m_valMPS);
    uint8_t range_id = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][range_id];
    range = range - rangeLPS;
    LOG(INFO, "range_id: %d\trangeLPS: %d\trange: %d\tlow: %d", range_id, rangeLPS, range, low);
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
    LOG(INFO, "BitStream: %s", out.c_str());
}

void putBit(uint8_t bit) {
    LOG(MAIN, "%s(%d).", __FUNCTION__, bit);
    if (firstBitFlag != 0) {
        firstBitFlag = 0;
    } else {
        //LOG(INFO, "%d", bit);
        out.append(std::to_string(bit));
    }
    while (bitsOut > 0) {
        //LOG(INFO, "%d", 1 - bit);
        out.append(std::to_string(1 - bit));
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

void CabacFsm::decodeBin() {
    LOG(MAIN, "%s()", __FUNCTION__);
    LOG(INFO, "range: %d\toffset: %d\tvalMPS: %d", range, low, m_valMPS);
    uint8_t range_id = static_cast<uint8_t>((range >> 6) & 3);
    uint32_t rangeLPS = rangeTabLPS[m_stateIdx][range_id];
    range = range - rangeLPS;
    LOG(INFO, "range_id: %d\trangeLPS: %d\trange: %d\tlow: %d", range_id, rangeLPS, range, low);

}

void CabacFsm::renormDecode() {

}

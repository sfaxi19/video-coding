//
// Created by sfaxi19 on 17.02.18.
//

#ifndef VIDEO_COMPRESSION_SCANNING_HPP
#define VIDEO_COMPRESSION_SCANNING_HPP


#include <motion_compensation/motion_compensation.h>

class Scanning {
private:
    mc::pos field[15] = {
            mc::pos(0, 1),
            mc::pos(1, 0),
            mc::pos(2, 0),
            mc::pos(1, 1),
            mc::pos(0, 2),
            mc::pos(0, 3),
            mc::pos(1, 2),
            mc::pos(2, 1),
            mc::pos(3, 0),
            mc::pos(3, 1),
            mc::pos(2, 2),
            mc::pos(1, 3),
            mc::pos(2, 3),
            mc::pos(3, 2),
            mc::pos(3, 3)
    };

    mc::pos zig_zag[15] = {
            mc::pos(1, 0),
            mc::pos(0, 1),
            mc::pos(0, 2),
            mc::pos(1, 1),
            mc::pos(2, 0),
            mc::pos(3, 0),
            mc::pos(2, 1),
            mc::pos(1, 2),
            mc::pos(0, 3),
            mc::pos(1, 3),
            mc::pos(2, 2),
            mc::pos(3, 1),
            mc::pos(3, 2),
            mc::pos(2, 3),
            mc::pos(3, 3)
    };
    bool m_isZig_zag = false;
    bool m_isField = false;
    bool m_stop = false;

    uint8_t m_state = 0;
public:

    static const uint8_t ZIG_ZAG = 0;
    static const uint8_t FIELD = 1;

    Scanning(uint8_t mode) {
        if (mode == ZIG_ZAG) {
            m_isZig_zag = true;
        } else {
            m_isField = true;
        }
    }

    mc::pos nextStep() {
        if (m_state >= 15) {
            return mc::pos(0, 0);
        }
        mc::pos p;
        if (m_isZig_zag) {
            p = zig_zag[m_state];
        } else {
            p = field[m_state];
        }
        m_state++;
        if (m_state >= 15) m_stop = true;
        return p;
    }

    bool stop() { return m_stop; }

    void reset() {
        m_stop = false;
        m_state = 0;
    }
};


#endif //VIDEO_COMPRESSION_SCANNING_HPP

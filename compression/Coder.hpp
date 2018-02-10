//
// Created by sfaxi19 on 28.12.17.
//

#ifndef AVI_MAKER_H264_HPP
#define AVI_MAKER_H264_HPP

class AVIMaker;

void avi_to_h264(AVIMaker &aviMaker);

void h264_to_avi(uint8_t *bytes, uint64_t &len, AVIMaker &aviMaker);

#endif //AVI_MAKER_H264_HPP

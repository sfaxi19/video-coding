//
// Created by sfaxi19 on 28.12.17.
//

#ifndef AVI_MAKER_H264_HPP
#define AVI_MAKER_H264_HPP

class AVIMaker;
class BitStream;

void coding(AVIMaker &aviMaker, BitStream& bitStream);

void decoding(AVIMaker &aviMaker, BitStream& bitStream);

#endif //AVI_MAKER_H264_HPP

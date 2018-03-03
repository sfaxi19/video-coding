//
// Created by sfaxi19 on 03.03.18.
//

#ifndef VIDEO_CODING_TOBITSTREAM_HPP
#define VIDEO_CODING_TOBITSTREAM_HPP

#include <Blocks.hpp>
#include "BitStream.hpp"

void BitStreamToMacroblockInfo(macroblock_info &mb_res, BitStream &bitStream);

void MacroblockInfoToBitStream(macroblock_info &mb_res, BitStream &bitStream);

#endif //VIDEO_CODING_TOBITSTREAM_HPP

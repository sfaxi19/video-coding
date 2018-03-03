cmake_minimum_required(VERSION 3.8)
project(video-coding)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp
        src/coding/Coder.cpp
        src/coding/Coder.hpp
        src/coding/CabacFsm.cpp
        src/coding/CabacFsm.hpp
        src/coding/Binarization.hpp
        src/coding/Binarization.cpp
        src/common/global.hpp
        src/coding/Scanning.cpp
        src/coding/Scanning.hpp
        src/coding/DCT.hpp
        src/coding/DCT.cpp
        src/coding/Quantization.cpp
        src/coding/Quantization.hpp
        src/coding/BitStream/BitStream.cpp
        src/coding/BitStream/BitStream.hpp src/Blocks.hpp src/Blocks.cpp src/coding/BitStream/BitStreamIO.hpp src/coding/BitStream/BitStreamIO.cpp)

add_executable(video-coding ${SOURCE_FILES})

include_directories(avi-maker/src)
include_directories(src)

add_subdirectory(avi-maker/)
target_link_libraries(video-coding  avi-maker-lib)
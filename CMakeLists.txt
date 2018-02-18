cmake_minimum_required(VERSION 3.8)
project(video-compression)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        compression/Coder.cpp
        compression/Coder.hpp
        compression/CabacFsm.cpp
        compression/CabacFsm.hpp
        compression/ExpGolombCodes.hpp
        compression/ExpGolombCodes.cpp
        compression/common/global.hpp
        compression/Scanning.cpp
        compression/Scanning.hpp
        compression/DCT.hpp
        compression/DCT.cpp
        compression/Quantization.cpp compression/Quantization.hpp compression/BitStream.cpp compression/BitStream.hpp)

add_executable(video-compression ${SOURCE_FILES})

include_directories(avi-maker/src)
add_subdirectory(avi-maker/)
target_link_libraries(video-compression  avi-maker-lib)
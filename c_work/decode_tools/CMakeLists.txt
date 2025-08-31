cmake_minimum_required(VERSION 4.0)
project(decode_tools)

set(CMAKE_CXX_STANDARD 20)

add_executable(decode_tools src/main.c
        src/decode_rtcm/rcvraw.c
        src/decode_rtcm/rtcm.c
        src/decode_rtcm/rtcm2.c
        src/decode_rtcm/rtcm3.c
        src/decode_rtcm/rtcm3e.c
        src/decode_rtcm/rtkcmn.c
        src/decode_rtcm/common.c
        src/config/config.h
        src/config/config_parser.c
        src/logger/logger.c
        src/logger/logger.h
        src/utils/file_utils.c
        src/utils/file_utils.h
        third_party/inih/ini.c
        src/protocols/protocol.c
        src/protocols/protocol.h
        src/protocols/rtcm_handler.c
        src/protocols/name_handler.c
)
target_link_libraries(decode_tools winmm ws2_32)
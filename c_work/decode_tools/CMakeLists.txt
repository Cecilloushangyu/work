cmake_minimum_required(VERSION 4.0)
project(decode_tools)

set(CMAKE_CXX_STANDARD 20)

add_executable(decode_tools main.c
        util/ini/ini.c
        src/decode_rtcm/rcvraw.c
        src/decode_rtcm/rtcm.c
        src/decode_rtcm/rtcm2.c
        src/decode_rtcm/rtcm3.c
        src/decode_rtcm/rtcm3e.c
        src/decode_rtcm/rtkcmn.c
        src/decode_rtcm/common.c
        src/process_input/input.c
        src/process_input/input.h
        src/process_output/log_manager.c
        src/process_output/log_manager.h
        src/process_input/config.c
        src/process_input/config.h
        src/process_input/log_types.h
        src/process_input/log_type.c
        src/process_input/file_utils.c
        src/process_input/file_utils.h
)
target_link_libraries(decode_tools winmm ws2_32)
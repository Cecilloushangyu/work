cmake_minimum_required(VERSION 4.0)
project(meas_to_rtcm C)

set(CMAKE_C_STANDARD 11)

add_executable(meas_to_rtcm
        main.c
        src/rtklib_src/rtklib.h
        src/rtklib_src/rtkcmn.c
        src/rtklib_src/rcvraw.c
        src/rtklib_src/rinex.c
        src/rtklib_src/rtcm.c
        src/rtklib_src/rcv/novatel.c
        src/rtklib_src/rcv/rt17.c
        src/rtklib_src/rtcm2.c
        src/rtklib_src/rtcm3.c
        src/rtklib_src/rtcm3e.c
)
target_link_libraries(meas_to_rtcm winmm)
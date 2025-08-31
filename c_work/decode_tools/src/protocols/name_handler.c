// src/protocols/nmea_handler.c
#include "protocol.h"
#include "../logger/logger.h"
#include <stdio.h>

// 简单 NMEA 状态机
static char nmea_buffer[256];
static int nmea_index = 0;
static int in_sentence = 0;

int nmea_probe(uint8_t byte, int pos) {
    // NMEA 以 '$' 开头
    return (pos == 0) && (byte == '$');
}

void nmea_consume(uint8_t byte) {

}

int nmea_init(void) {
    nmea_index = 0;
    in_sentence = 0;
    return 1;
}

void nmea_close(void) {
    // 清理
}

protocol_handler_t nmea_handler = {
        .name    = "NMEA",
        .probe   = nmea_probe,
        .consume = nmea_consume,
        .init    = nmea_init,
        .close   = nmea_close
};
// src/protocols/rtcm_handler.c
#include "protocol.h"
#include "../decode_rtcm/rtklib.h"
#include <string.h>

// 全局 rtcm 结构（也可改为动态分配）
static rtcm_t rtcm = {0};

// RTCM 探测：第一个字节是 0xD3
int rtcm_probe(uint8_t byte, int pos) {
    return byte == 0xD3;
}

// RTCM 解析
void rtcm_consume(uint8_t byte) {
    input_rtcm3(&rtcm, byte);
}

int rtcm_init(void) {
    return init_rtcm(&rtcm) ? 1 : 0;
}

void rtcm_close(void) {
    free_rtcm(&rtcm);
    memset(&rtcm, 0, sizeof(rtcm));
}

// 注册 RTCM 处理器
protocol_handler_t rtcm_handler = {
        .name    = "RTCM3",
        .probe   = rtcm_probe,
        .consume = rtcm_consume,
        .init    = rtcm_init,
        .close   = rtcm_close
};
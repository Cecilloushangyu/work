// src/protocols/protocol.c
#include "protocol.h"
#include <stdio.h>

// 外部声明各协议处理器
extern protocol_handler_t rtcm_handler;
extern protocol_handler_t nmea_handler;
// extern protocol_handler_t ubx_handler;

// 所有协议处理器注册表
static const protocol_handler_t* handlers[PROTO_MAX] = {0};

// 当前猜测的协议
static protocol_type_t current_proto = PROTO_UNKNOWN;

// 当前字节位置（用于探测）
static int g_pos = 0;

void protocol_init(void) {
    // 注册所有支持的协议
    handlers[PROTO_RTCM3] = &rtcm_handler;
    handlers[PROTO_NMEA]  = &nmea_handler;
    // handlers[PROTO_UBX]   = &ubx_handler;

    // 初始化所有协议
    for (int i = 0; i < PROTO_MAX; i++) {
        if (handlers[i] && handlers[i]->init) {
            handlers[i]->init();
        }
    }

    current_proto = PROTO_UNKNOWN;
    g_pos = 0;
}

void protocol_consume(uint8_t byte) {
    // 1. 如果已有确定协议，直接 consume，不再探测
    if (current_proto != PROTO_UNKNOWN &&
        handlers[current_proto] &&
        handlers[current_proto]->consume) {

        handlers[current_proto]->consume(byte);
        g_pos++;  // 更新位置
        return;
    }

    // 2. 否则：尝试探测
    protocol_type_t proto = protocol_detect(byte, g_pos);

    if (proto != PROTO_UNKNOWN && handlers[proto]) {
        current_proto = proto;
        handlers[proto]->consume(byte);  // 第一个字节也要处理
        g_pos++;  // 更新位置
        return;
    }

    // 3. 未知数据，跳过
    g_pos++;
}

protocol_type_t protocol_detect(uint8_t byte, int pos) {
    int candidates = 0;
    protocol_type_t detected = PROTO_UNKNOWN;

    for (int i = 0; i < PROTO_MAX; i++) {
        if (handlers[i] && handlers[i]->probe) {
            if (handlers[i]->probe(byte, pos)) {
                detected = i;
                candidates++;
                // 可改进：支持多个匹配，按优先级选择
            }
        }
    }

    // 简单策略：仅当唯一匹配时才确定
    return (candidates == 1) ? detected : PROTO_UNKNOWN;
}

void protocol_close(void) {
    for (int i = 0; i < PROTO_MAX; i++) {
        if (handlers[i] && handlers[i]->close) {
            handlers[i]->close();
        }
    }
}

// 可选：重置协议探测器（如处理多个文件时）
void protocol_reset(void) {
    current_proto = PROTO_UNKNOWN;
    g_pos = 0;
}
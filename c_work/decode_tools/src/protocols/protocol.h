// src/protocols/protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// 协议类型枚举
typedef enum {
    PROTO_UNKNOWN = 0,
    PROTO_RTCM3,
    PROTO_NMEA,
    PROTO_UBX,
    PROTO_CUSTOM,
    PROTO_MAX
} protocol_type_t;

// 协议处理器函数指针
typedef struct {
    const char* name;

    // 判断一个字节是否可能属于本协议（用于探测）
    int (*probe)(uint8_t byte, int pos);

    // 完整解析一个字节（状态机）
    void (*consume)(uint8_t byte);

    // 初始化
    int (*init)(void);

    // 清理
    void (*close)(void);
} protocol_handler_t;

// 全局函数
void protocol_init(void);                    // 初始化所有协议
void protocol_consume(uint8_t byte);         // 统一入口：传入字节
void protocol_close(void);                   // 清理所有协议

protocol_type_t protocol_detect(uint8_t byte, int pos);  // 探测协议

#endif
// input.c
#include "input.h"
#include "log_types.h"
#include <string.h>
#include "../decode_rtcm/rtklib.h"
// ==================== 外部声明：你的函数和状态变量 ====================

// 函数声明（保持原名）
extern int input_rtcm3(rtcm_t *rtcm, uint8_t data);
// extern int input_lidar(void* state, uint8_t data); // 将来加

// 状态变量声明（必须在别处定义）
extern rtcm_t      rtcm;
// extern lidar_data_t lidar_data; // 将来加

// ==================== 手动注册解析器表 ====================

// ✅ 核心：手动映射 "类型名" -> 函数 -> 状态变量
parser_entry_t parsers[] = {
        {
                .name    = "RTCMMSM",           // 配置文件中的名字
                .func    = (input_func_t)input_rtcm3,  // 你不能改的函数
                .state   = &rtcm,               // 状态变量
                .enabled = 0                    // 运行时由配置启用
        },
        // 将来加 LIDAR？在这里加一行
        // {
        //     .name    = "LIDAR",
        //     .func    = input_lidar,
        //     .state   = &lidar_data,
        //     .enabled = 0
        // }
};

int parser_count = sizeof(parsers) / sizeof(parsers[0]);

// 根据配置启用/禁用解析器
int setup_parsers_from_config(char (*type_names)[MAX_NAME_LEN], int* enabled, int count) {
    // 先全部禁用
    for (int i = 0; i < parser_count; i++) {
        parsers[i].enabled = 0;
    }

    // 遍历配置项
    for (int i = 0; i < count; i++) {
        const char* name = type_names[i];
        int enable = enabled[i];

        for (int j = 0; j < parser_count; j++) {
            if (strcmp(parsers[j].name, name) == 0) {
                parsers[j].enabled = enable;
                break;
            }
        }
    }

    return 0;
}

// 主处理函数：喂一个字节给所有启用的解析器
void process_byte(uint8_t data) {
    for (int i = 0; i < parser_count; i++) {
        if (parsers[i].enabled && parsers[i].func) {
            parsers[i].func(parsers[i].state, data);
        }
    }
}
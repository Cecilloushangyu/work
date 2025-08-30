// log_manager.c
#include "log_manager.h"
#include "../process_input/log_types.h"  // 确保 LOG_TYPE_COUNT 已定义
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../process_input/config.h"

// ✅ 正确定义 type_map 数组
static struct {
    const char* name;
    log_type_t  type;
} type_map[] = {
        { "RTCMMSM", LOG_TYPE_RTCMMSM },
        { "PVT",     LOG_TYPE_PVT     },
        { "RAWIMU",  LOG_TYPE_RAWIMU  },
        // 添加更多...
};

#define TYPE_MAP_COUNT (sizeof(type_map) / sizeof(type_map[0]))
// ==================== 函数实现 ====================

log_type_t get_log_type(const char* name) {
    if (!name) return -1;
    for (int i = 0; i < TYPE_MAP_COUNT; i++) {
        if (strcmp(type_map[i].name, name) == 0) {
            return type_map[i].type;
        }
    }
    return -1;
}
static FILE* log_files[LOG_TYPE_COUNT] = {0};
static int   log_enabled[LOG_TYPE_COUNT] = {0};
int init_log_manager(Config* cfg) {
    for (int i = 0; i < LOG_TYPE_COUNT; i++) {
        log_files[i] = NULL;
        log_enabled[i] = 0;
    }

    for (int i = 0; i < cfg->type_count; i++) {
        log_type_t type = get_log_type(cfg->extract_types[i]);
        if (type >= 0 && type < LOG_TYPE_COUNT) {
            log_enabled[type] = cfg->type_enabled[i];
            log_files[type] = cfg->log_files[i];
        }
    }
    return 0;
}


int write_log_line(log_type_t type, const char* format, ...) {
    if (type < 0 || type >= LOG_TYPE_COUNT) return -1;
    if (!log_enabled[type] || !log_files[type]) return -1;

    va_list args;
    va_start(args, format);
    vfprintf(log_files[type], format, args);
    va_end(args);

    fprintf(log_files[type], "\n");
    fflush(log_files[type]);

    return 0;
}
void close_log_manager(void) {
    for (int i = 0; i < LOG_TYPE_COUNT; i++) {
        if (log_files[i] && log_files[i] != stdout && log_files[i] != stderr) {
            fclose(log_files[i]);
        }
        log_files[i] = NULL;
    }
}
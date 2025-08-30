// log_manager.h
#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stdio.h>
#include "../process_input/log_types.h"  // ✅ 先包含类型定义
#include "../process_input/config.h"
#ifdef __cplusplus
extern "C" {
#endif

// 函数声明
log_type_t get_log_type(const char* name);
int init_log_manager(Config* cfg);
int write_log_line(log_type_t type, const char* format, ...);
void close_log_manager(void);

#ifdef __cplusplus
}
#endif

#endif // LOG_MANAGER_H
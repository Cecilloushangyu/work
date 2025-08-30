// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#define MAX_PATH_LEN    1024
#define MAX_TYPES       20
#define MAX_NAME_LEN    32

typedef struct {
    // --- 输入配置 ---
    char input_file[MAX_PATH_LEN];          // 原 decode_file

    // --- 提取配置 ---
    char extract_types[MAX_TYPES][MAX_NAME_LEN]; // 原 decode_types
    int  type_enabled[MAX_TYPES];           // 是否启用提取
    int  type_count;                        // 类型总数

    // --- 输出配置（可扩展）---
    char output_dir[MAX_PATH_LEN];          // 原 dir_name，默认 "log_output"

    // --- 日志文件管理（供 log_manager 使用）---
    FILE* log_files[MAX_TYPES];
    char  log_file_paths[MAX_TYPES][MAX_PATH_LEN];

} Config;

extern int handler(void* user, const char* section, const char* name, const char* value);

#endif
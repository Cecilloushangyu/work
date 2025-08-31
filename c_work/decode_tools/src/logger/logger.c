// src/logger/logger.c
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../config/config.h"  // 引入 g_settings

// ✅ 使用 config.h 中的 output_files 数组，不再用 enum 索引
static FILE* g_log_files[MAX_OUTPUT_FILES] = {0};  // 与 g_settings.output_files 对应

int init_loggers(const char *output_dir) {
    for (int i = 0; i < g_settings.output_file_count; i++) {
        const OutputFile* file_cfg = &g_settings.output_files[i];
        if (!file_cfg->enabled) continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", output_dir, file_cfg->name);

        // 打开文件
        FILE* fp = fopen(path, "w");
        if (!fp) {
            perror(path);
            // 清理已打开的
            for (int j = 0; j < i; j++) {
                if (g_log_files[j]) {
                    fclose(g_log_files[j]);
                    g_log_files[j] = NULL;
                }
            }
            return -1;
        }
        g_log_files[i] = fp;
    }
    return 0;
}

// ✅ 核心：通过文件名查找并写入
void log_write(const char* filename, const char* fmt, ...) {
    // 在 g_settings.output_files 中查找 filename
    for (int i = 0; i < g_settings.output_file_count; i++) {
        const OutputFile* file_cfg = &g_settings.output_files[i];
        if (file_cfg->enabled && strcmp(file_cfg->name, filename) == 0) {
            if (g_log_files[i]) {
                va_list args;
                va_start(args, fmt);
                vfprintf(g_log_files[i], fmt, args);
                va_end(args);
                fputc('\n', g_log_files[i]);
                fflush(g_log_files[i]);
            }
            return;  // 找到并写入，退出
        }
    }
    // 如果没找到，说明未启用或未配置，静默丢弃
}

void close_loggers(void) {
    for (int i = 0; i < g_settings.output_file_count; i++) {
        if (g_log_files[i]) {
            fclose(g_log_files[i]);
            g_log_files[i] = NULL;
        }
    }
}
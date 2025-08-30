// handler.c 或 main.c 中
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>  // _beginthreadex
#include "config.h" // 包含更新后的结构体定义

extern int handler(void* user, const char* section, const char* name, const char* value) {
    Config* cfg = (Config*)user;

    if (strcmp(section, "Input") == 0) {
        if (strcmp(name, "file") == 0) {
            strncpy(cfg->input_file, value, sizeof(cfg->input_file) - 1);
            cfg->input_file[sizeof(cfg->input_file) - 1] = '\0';
        }
    }
    else if (strcmp(section, "Output") == 0) {
        if (strcmp(name, "dir_name") == 0) {
            strncpy(cfg->output_dir, value, sizeof(cfg->output_dir) - 1);
            cfg->output_dir[sizeof(cfg->output_dir) - 1] = '\0';
        }
    }
    else if (strcmp(section, "ExtractTypes") == 0) {
        if (cfg->type_count < MAX_TYPES) {
            strncpy(cfg->extract_types[cfg->type_count], name, MAX_NAME_LEN - 1);
            cfg->extract_types[cfg->type_count][MAX_NAME_LEN - 1] = '\0';

            cfg->type_enabled[cfg->type_count] = (atoi(value) != 0);
            cfg->type_count++;
        }
    }

    return 1;
}
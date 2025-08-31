// src/config/config_parser.c
#include "config.h"
#include "../../third_party/inih/ini.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

AppSettings g_settings = {0};

static int config_handler(void *user, const char *section, const char *name, const char *value) {
#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

    if (MATCH("Input", "file")) {
        strncpy(g_settings.input_file, value, sizeof(g_settings.input_file) - 1);
        g_settings.input_file[sizeof(g_settings.input_file) - 1] = '\0';
    }
    else if (MATCH("Output", "dir_name")) {
        strncpy(g_settings.output_dir, value, sizeof(g_settings.output_dir) - 1);
        g_settings.output_dir[sizeof(g_settings.output_dir) - 1] = '\0';
    }
    else if (MATCH("DecodeType", "RTCM")) {
        g_settings.decode_rtcm = (atoi(value) != 0);
    }
    else if (MATCH("DecodeType", "NMEA")) {
        g_settings.decode_nmea = (atoi(value) != 0);
    }
        // 新增：处理 [LogFile] 下的所有键
    else if (strcmp(section, "LogFile") == 0) {
        if (g_settings.output_file_count >= MAX_OUTPUT_FILES) {
            fprintf(stderr, "Too many output files in config\n");
            return 0;
        }
        OutputFile* file = &g_settings.output_files[g_settings.output_file_count++];
        // 复制文件名（即键名）
        strncpy(file->name, name, MAX_FILENAME_LEN - 1);
        file->name[MAX_FILENAME_LEN - 1] = '\0';  // 确保字符串结束
        // 启用标志：1 为启用
        file->enabled = (atoi(value) != 0);
    }

    return 1;  // 继续解析
}

int load_config(const char *config_path) {
    if (ini_parse(config_path, config_handler, NULL) < 0) {
        fprintf(stderr, "Can't load '%s'\n", config_path);
        return 0;
    }
    if (g_settings.input_file[0] == '\0') {
        fprintf(stderr, "Missing [Input] file in config\n");
        return 0;
    }
    if (g_settings.output_dir[0] == '\0') {
        strcpy(g_settings.output_dir, "log_output");  // 默认
    }
    return 1;
}
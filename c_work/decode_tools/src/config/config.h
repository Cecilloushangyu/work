// src/config/config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define MAX_OUTPUT_FILES 32
#define MAX_FILENAME_LEN 64
typedef struct {
    char name[MAX_FILENAME_LEN];
    int enabled;
} OutputFile;
typedef struct {
    char input_file[512];
    char output_dir[256];
    int decode_rtcm;  // 来自 [DecodeType] RTCM=1
    int decode_nmea;  // 来自 [DecodeType] NMEA=1

    OutputFile output_files[MAX_OUTPUT_FILES];
    int output_file_count;  // 当前输出文件数量
} AppSettings;

// 全局配置（extern）
extern AppSettings g_settings;

int load_config(const char *config_path);

#endif
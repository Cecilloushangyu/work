// src/logger/logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// 删除 enum LogType 和 LOG_FILENAMES[]
// #define LOG_TYPE_COUNT 2
// typedef enum { LOG_RTCM, LOG_NMEA } LogType;
// extern const char* LOG_FILENAMES[LOG_TYPE_COUNT];

// 改为：通过文件名操作
void log_write(const char* filename, const char* fmt, ...);
int init_loggers(const char* output_dir);
void close_loggers(void);

#endif
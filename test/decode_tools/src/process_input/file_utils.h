//
// Created by HUAWEI on 2025/8/31.
//

#ifndef DECODE_TOOLS_FILE_UTILS_H
#define DECODE_TOOLS_FILE_UTILS_H
#include "config.h"
int setup_output_directory(Config* config);
int open_log_files(Config* config);
void process_input_file(const char* filename);
#endif //DECODE_TOOLS_FILE_UTILS_H

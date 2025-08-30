//
// Created by HUAWEI on 2025/8/30.
//
#include "rtklib.h"

#include <stdio.h>
#include <stdarg.h>

extern int showmsg(const char *format, ...) {
    va_list args;           // 定义参数列表
    va_start(args, format); // 初始化，从 format 开始读取后续参数

    int result = vprintf(format, args); // 使用 vprintf 处理可变参数

    va_end(args); // 清理

    return result; // 返回 printf 的返回值（打印的字符数）
}

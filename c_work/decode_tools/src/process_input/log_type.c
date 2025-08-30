// src/process_input/log_types.c
#include "log_types.h"

// ✅ 定义：只在这里实现一次
const char* log_type_names[] = {
        [LOG_TYPE_RTCMMSM] = "RTCMMSM",
        [LOG_TYPE_PVT]     = "PVT",
        [LOG_TYPE_RAWIMU]  = "RAWIMU",
};
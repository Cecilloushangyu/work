// log_types.h
#ifndef LOG_TYPES_H
#define LOG_TYPES_H

typedef enum {
    LOG_TYPE_RTCMMSM,
    LOG_TYPE_PVT,
    LOG_TYPE_RAWIMU,
    LOG_TYPE_COUNT
} log_type_t;
#undef LOG_TYPE
extern const char* log_type_names[];
#endif // LOG_TYPES_H
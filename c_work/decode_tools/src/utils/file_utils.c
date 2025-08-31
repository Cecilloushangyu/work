// src/utils/file_utils.c
#include "file_utils.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define ACCESS _access
#else
#include <unistd.h>
    #define MKDIR(path) mkdir(path, 0755)
    #define ACCESS access
#endif

int create_directory_if_not_exists(const char *path) {
    if (ACCESS(path, 0) == 0) return 1;  // 已存在
    if (MKDIR(path) == 0) return 1;      // 创建成功
    return 0;
}

int get_dir_from_filepath(const char *file, char *dir, size_t size) {
    const char *sep = strrchr(file, '\\');
    if (!sep) sep = strrchr(file, '/');
    if (!sep) return 0;
    size_t len = sep - file;
    if (len >= size) return 0;
    memcpy(dir, file, len);
    dir[len] = '\0';
    return 1;
}
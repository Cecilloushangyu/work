
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>  // _mkdir
#include <io.h>      // _access
#else
#include <sys/stat.h> // mkdir, access
    #include <unistd.h>   // access
#endif

#include "config.h"
#include "input.h"
#include "file_utils.h"

// 定义跨平台宏
#ifdef _WIN32
#define MKDIR(path) _mkdir(path)
#define ACCESS(path) _access(path, 0)
#else
#define MKDIR(path) mkdir(path, 0755)
    #define ACCESS(path) access(path, F_OK)
#endif
// 从文件路径提取目录（不包含文件名）
static void get_dir_path(const char* filepath, char* dir, size_t size) {
    if (strlen(filepath) >= size) {
        strncpy(dir, filepath, size - 1);
        dir[size - 1] = '\0';
    } else {
        strcpy(dir, filepath);
    }

    char* slash = strrchr(dir, '/');
#ifdef _WIN32
    char* backslash = strrchr(dir, '\\');
    if (backslash && (!slash || backslash > slash)) slash = backslash;
#endif
    if (slash) {
        *slash = '\0';
    } else {
        strcpy(dir, ".");  // 无路径则为当前目录
    }
}

// ✅ 增强版：设置输出目录并返回完整路径
int setup_output_directory(Config* config) {
    char input_dir[512] = {0};
    get_dir_path(config->input_file, input_dir, sizeof(input_dir));

    // ✅ 打印调试信息
    printf("Input file: %s\n", config->input_file);
    printf("Input directory: %s\n", input_dir);

    // 构造 log_output 路径
    snprintf(config->output_dir, sizeof(config->output_dir), "%s/%s", input_dir, "log_output");
    printf("Will create log directory: %s\n", config->output_dir);

    // ✅ 检查目录是否已存在
    if (ACCESS(config->output_dir) == 0) {
        printf("Log directory already exists.\n");
        return 0;
    }

    // ✅ 尝试创建
    int result = MKDIR(config->output_dir);
    if (result == 0) {
        printf("Successfully created log directory.\n");
        return 0;
    } else {
        // ✅ 打印错误原因
        printf("Failed to create directory '%s'\n", config->output_dir);
#ifdef _WIN32
        if (errno == 183) {
            printf("   Reason: Directory already exists (ERROR_ALREADY_EXISTS)\n");
        } else if (errno == 2) {
            printf("   Reason: Path not found. Check if '%s' exists.\n", input_dir);
        } else if (errno == 13) {
            printf("   Reason: Permission denied.\n");
        } else {
            printf("   errno = %d\n", errno);
        }
#else
        perror("   Reason");
#endif
        return -1;
    }
}

// 打开所有启用的日志类型的文件
int open_log_files(Config* config) {
    for (int i = 0; i < config->type_count; i++) {
        if (!config->type_enabled[i]) {
            config->log_files[i] = NULL;
            continue;
        }

        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s.log", config->output_dir, config->extract_types[i]);

        config->log_files[i] = fopen(filepath, "w");
        if (!config->log_files[i]) return -1;
    }
    return 0;
}

// 处理输入文件
void process_input_file(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return;

    uint8_t byte;
    while (fread(&byte, 1, 1, fp) == 1) {
        process_byte(byte);
    }
    fclose(fp);
}
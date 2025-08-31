// src/main.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>  // for isatty()
#include "config/config.h"
#include "logger/logger.h"
#include "utils/file_utils.h"
#include "protocols/protocol.h"

int main(int argc, char *argv[]) {
    const char *config_path = "../config.ini";

    if (!load_config(config_path)) {
        fprintf(stderr, "Failed to load config\n");
        return -1;
    }

    char output_dir[1024] = {0};
    get_dir_from_filepath(g_settings.input_file, output_dir, sizeof(output_dir));
    strcat(output_dir, "/");
    strcat(output_dir, g_settings.output_dir);

    create_directory_if_not_exists(output_dir);
    init_loggers(output_dir);

    FILE *fp = fopen(g_settings.input_file, "rb");
    if (!fp) {
        perror("Open file");
        close_loggers();
        return -1;
    }

    // 🔢 获取文件总大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ⚠️ 防止除以 0
    if (file_size == 0) {
        fprintf(stderr, "Empty file\n");
        fclose(fp);
        close_loggers();
        return -1;
    }

    // 🔥 核心变化：初始化协议分发器
    protocol_init();

    uint8_t byte;
    long bytes_read = 0;

    // 📊 进度显示：每 1% 更新一次
    int last_percent = -1;

    printf("Processing: 0%%\r");
    fflush(stdout);  // 确保立即输出

    while (fread(&byte, 1, 1, fp) == 1) {
        protocol_consume(byte);
        bytes_read++;

        // 📈 计算进度百分比
        int percent = (int)(100.0 * bytes_read / file_size);

        // 🖨️ 每 1% 更新一次（减少刷屏）
        if (percent != last_percent && percent % 1 == 0) {
            // 仅在终端中显示进度条，重定向到文件时不输出
            if (isatty(STDOUT_FILENO)) {
                printf("Processing: %d%% (%ld/%ld) bytes\r", percent, bytes_read, file_size);
                fflush(stdout);
            } else {
                // 非终端：每 10% 打印一行（避免日志污染）
                if (percent % 10 == 0) {
                    fprintf(stderr, "Progress: %d%%\n", percent);
                }
            }
            last_percent = percent;
        }
    }

    fclose(fp);

    // ✅ 确保最终显示 100%
    if (isatty(STDOUT_FILENO)) {
        printf("Processing: 100%% (%ld/%ld) bytes\n", bytes_read, file_size);
    } else {
        fprintf(stderr, "Progress: 100%%\n");
    }

    printf("Parsing completed.\n");

    // 清理
    protocol_close();
    close_loggers();

    return 0;
}
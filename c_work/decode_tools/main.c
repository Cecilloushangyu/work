// main.c
#include "util/ini/ini.h"
#include "src/process_input/config.h"
#include "src/process_input/input.h"
#include "src/process_output/log_manager.h"
#include "src/decode_rtcm/rtklib.h"
#include "src/process_input/file_utils.h"
// 声明外部函数
extern int handler(void* user, const char* section, const char* name, const char* value);

int main() {
    Config config = {0};

    // 1. 解析配置
    if (ini_parse("../config.ini", handler, &config) < 0) return 1;

    // 2. 设置输出目录（与输入文件同目录）
    if (setup_output_directory(&config) != 0) return 1;

    // 3. 初始化 RTCM 解析器
    if (!init_rtcm(&rtcm)) return 1;

    // 4. 启用指定解析器
    setup_parsers_from_config(
            (const char**)config.extract_types,
            config.type_enabled,
            config.type_count
    );

    // 5. 打开日志文件
    if (open_log_files(&config) != 0) return 1;

    // 6. 初始化日志管理器
    if (init_log_manager(&config) != 0) return 1;

    // 7. 处理输入数据
    process_input_file(config.input_file);

    // 8. 清理资源
    free_rtcm(&rtcm);
    close_log_manager();

    return 0;
}
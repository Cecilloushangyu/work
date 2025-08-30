// input.h
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "../process_input/config.h"
#ifdef __cplusplus
extern "C" {
#endif

// 输入处理函数类型
typedef int (*input_func_t)(void* state, uint8_t data);

// 状态结构
typedef struct {
    const char* name;
    input_func_t func;
    void* state;
    int enabled;
} parser_entry_t;

// 外部声明：由 parser_registry 自动生成
extern parser_entry_t parsers[];
extern int parser_count;

// 函数声明
int setup_parsers_from_config(char (*type_names)[MAX_NAME_LEN], int* enabled, int count);
void process_byte(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
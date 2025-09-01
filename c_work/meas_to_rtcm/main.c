#include <stdio.h>
#include <stdlib.h>
#include "src/rtklib_src/rtklib.h"
char *filename="D:\\work\\myproject\\work\\c_work\\meas_to_rtcm\\testlog\\test.log";

int main() {
    FILE *file;
    char buffer[100];

    // 以二进制只读方式打开文件
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("open error");
        return 1;
    }
    raw_t raw;
    memset(&raw,0,sizeof(raw_t));
    init_raw(&raw,STRFMT_OEM4);
    int stat;
    int n = 0;
    while ((stat = input_rawf(&raw, STRFMT_OEM4, file)) >= -1) {

    }
    free_raw(&raw);
    // 关闭文件
    fclose(file);
    return 0;
}
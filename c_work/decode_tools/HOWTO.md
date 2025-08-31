---

### ✅ 文件名：`HOWTO.md`

```markdown
# 📄 如何新增协议或消息类型

本系统采用 **配置驱动 + 插件式协议解析** 架构。所有功能由 `config.ini` 控制。

> **核心流程**：
> ```
> config.ini → main.c → protocol.c → 协议处理器（RTCM/NMEA） → 消息解析
> ```

---

## 1. 配置文件：`config.ini`

所有输入、输出、日志开关均由 `config.ini` 控制。

### 示例配置

```ini
[Input]
input_file = ../data/input.bin

[Output]
output_dir = output/

[LogFile]
RTCM_EPH.txt = 1
RTCM_RAW.txt = 1
NMEA_GGA.txt = 1
```

### 说明

| 配置项 | 作用 |
|--------|------|
| `[Input]` | 指定输入数据文件路径 |
| `[Output]` | 指定输出日志目录 |
| `[LogFile]` | **控制日志文件是否启用**（`1`=启用，`0`=禁用） |

> 💡 **提示**：  
> 想输出新日志？只需在 `[LogFile]` 中添加一行，例如：
> ```ini
> RTCM_1042.txt = 1
> ```

---

## 2. 如何新增一种协议（如 NMEA）

当您想支持 **NMEA-0183** 等新协议时，请按以下步骤操作。

### 步骤 1：创建协议处理器文件

```bash
cp src/protocols/rtcm_handler.c src/protocols/nmea_handler.c
```

### 步骤 2：实现协议逻辑（`nmea_handler.c`）

```c
#include "protocol.h"

// 探测函数：判断是否为 NMEA 数据（以 '$' 开头）
int nmea_probe(uint8_t byte, int pos) {
    return (pos == 0 && byte == '$');
}

// 消费函数：逐字节处理 NMEA 数据
void nmea_consume(uint8_t byte) {
    input_nmea(byte);  // 假设您有 NMEA 解析函数
}

// 初始化（可选）
int nmea_init(void) { return 1; }

// 清理（可选）
void nmea_close(void) {}

// 注册处理器
protocol_handler_t nmea_handler = {
    .name    = "NMEA",
    .probe   = nmea_probe,
    .consume = nmea_consume,
    .init    = nmea_init,
    .close   = nmea_close
};
```

### 步骤 3：在 `protocol.c` 中注册

```c
// protocol.c
extern protocol_handler_t nmea_handler;  // 声明外部处理器

static const protocol_handler_t* handlers[PROTO_MAX] = {
    [PROTO_RTCM3] = &rtcm_handler,
    [PROTO_NMEA]  = &nmea_handler,  // ← 新增注册
};
```

### 步骤 4：定义协议类型（`protocol.h`）

```c
typedef enum {
    PROTO_UNKNOWN = 0,
    PROTO_RTCM3,
    PROTO_NMEA,   // ← 新增
    PROTO_MAX
} protocol_type_t;
```

✅ **完成！** 系统将自动识别并解析 NMEA 数据。

---

## 3. 如何在 RTCM 中新增消息类型（如 1042）

当您想解析 **RTCM 1042**（BDS 星历）等消息时，请按以下步骤操作。

### 步骤 1：添加解析函数（`rtcm_handler.c`）

```c
static int decode_type1042(rtcm_t *rtcm) {
    int prn  = getbitu(rtcm->buff, 24+12,   6);  // PRN
    int week = getbitu(rtcm->buff, 24+12+6, 13); // 周数

    // ... 解析其他字段 ...

    // 写入日志（文件名需在 config.ini 中启用）
    log_write("RTCM_1042.txt", "$EPH1042,%d,%d", prn, week);
    return 1;
}
```

### 步骤 2：在 `input_rtcm3` 中注册消息 ID

```c
// rtcm_handler.c
int input_rtcm3(rtcm_t *rtcm, uint8_t data) {
    // ... 解包逻辑 ...

    switch (msg_type) {
        case 1042:
            decode_type1042(rtcm);
            break;

        // 其他 case ...
    }
    return 0;
}
```

### 步骤 3：启用日志输出（`config.ini`）

```ini
[LogFile]
RTCM_1042.txt = 1  # 启用日志输出
```

✅ **完成！** 系统将把 RTCM 1042 消息写入 `output/RTCM_1042.txt`。

---

## ✅ 总结：新增功能三步走

| 目标 | 操作步骤 |
|------|----------|
| **新增协议**<br>（如 NMEA） | 1. 创建 `xxx_handler.c`<br>2. 实现 `probe` + `consume`<br>3. 在 `protocol.c` 中注册 |
| **新增消息类型**<br>（如 RTCM 1042） | 1. 添加 `decode_typeXXX()` 函数<br>2. 在 `switch` 中添加 `case`<br>3. 在 `config.ini` 中启用日志 |

> 💡 **关键原则**：  
> 所有日志使用 `log_write("filename.txt", "...")`，  
> **文件名必须在 `config.ini` 的 `[LogFile]` 中设置为 `1`**，否则不会输出。

---
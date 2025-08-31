
# 🛰️ GNSS 协议解析器（RTCM/NMEA 解析工具）

一个轻量、可扩展的 GNSS 数据解析器，支持 **RTCM 3.x** 和 **NMEA-0183** 等协议，适用于日志分析、差分数据处理、星历提取等场景。

> 📌 核心特性：**配置驱动** + **插件式协议** + **灵活日志输出**

---

## 🚀 功能特点

- ✅ 支持多种 GNSS 协议：RTCM 3.x、NMEA（可扩展）
- ✅ 自动协议探测（无需手动指定格式）
- ✅ 高性能逐字节流式解析
- ✅ 配置驱动：通过 `config.ini` 控制输入、输出和日志
- ✅ 模块化设计：轻松新增协议或消息类型
- ✅ 进度显示：大文件处理不黑屏
- ✅ 日志分级：按需输出，便于调试与分析

---

## 📁 项目结构

```text
decode-tools/
├── config.ini           # 主配置文件
├── HOWTO.md             # 新增协议/消息类型指南
├── REAME.md             # 项目介绍
├── CMakeLists.txt       # CMake 配置文件
├── src/
│   ├── main.c           #主程序
│   ├── protocols/       #协议处理模块
│   ├── decode_rtcm/     #RTCM 处理模块
│   ├── config/          #配置模块
│   ├── logger/          #日志模块
│   └── utils/           #工具模块
├── doc/                 # 文档
└── third_party/         # 第三方库
```

---

## ⚙️ 使用方法

### 1. 配置 `config.ini`

```ini
# =============================================
# decode_tools 配置文件
# 功能：指定输入文件，并选择需要提取哪些内部消息类型
# =============================================

[Input]
# 要解析的输入日志文件路径
file = D:\work\myproject\work\c_work\decode_tools\testlog\test.log

[Output]
# 输出目录名称（固定或可配置）
dir_name = log_output

[LogFile]
rtcmmsm.txt = 1
rtcmeph.txt = 0
nmea.txt = 0

[DecodeType]
# 启用或禁用特定类型的消息解码
RTCM = 1
NMEA = 0
```

> ✅ 启用哪个日志，就设为 `1`

---

### 2. 编译项目

```bash
# 使用 gcc 编译（示例）
gcc -o parser src/main.c src/protocols/rtcm_handler.c src/protocols/nmea_handler.c \
          src/utils/file_utils.c src/logger/logger.c \
          -lm -D_LINUX_

# 或使用 Makefile/CMake（推荐）
make
```

---

### 3. 运行解析器

```bash
./parser
```

输出示例：
```text
Processing: 73% (7,356,210/10,000,000) bytes
Parsing completed.
```

---

### 4. 查看输出日志

解析结果将保存在 `output/` 目录下：

```text
output/
├── rtcmmsm.txt     # 所有 RTCM 消息
├── rtcmeph.txt     # 星历信息
└── nmea.txt     # NMEA 定位句子
```

---

## 📚 扩展开发

想新增一种协议（如 UBX）或消息类型（如 RTCM 1042）？

👉 请查看详细指南：  
[**HOWTO.md**](HOWTO.md) —— 从配置到代码，手把手教学。

---

## 🧩 适用场景

- 差分 GNSS 数据分析
- 星历提取与验证
- 基站日志解析
- 算法调试与测试
- 多协议融合处理

---

## 📬 反馈与贡献

欢迎提交 Issue 或 Pull Request！

> 项目目标：简单、清晰、可维护。
```

---
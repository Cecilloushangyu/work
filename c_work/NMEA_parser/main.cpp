#include <iostream>
#include <fstream>  // 用于文件操作
#include <sstream>  // 用于字符串流 (在 parseGGA 中使用)
#include <string>
#include <vector>
#include <iomanip> // 用于 std::setprecision
#include <cctype>  // 用于 std::isxdigit

// 定义存储 GGA 数据的结构体
struct GGAData {
    std::string message_type; // 消息类型 (GNGGA or GPGGA)
    std::string time_utc;     // UTC 时间 (hhmmss.sss)
    double latitude;          // 纬度 (度)
    char lat_hemisphere;      // 纬度半球 (N/S)
    double longitude;         // 经度 (度)
    char lon_hemisphere;      // 经度半球 (E/W)
    int fix_quality;           // 定位质量指示
    int num_satellites;       // 使用卫星数
    double hdop;              // 水平精度因子
    double altitude;          // 海拔高度 (米)
    std::string altitude_unit; // 海拔单位 (通常为 M)
    double geoid_height;       // 大地水准面高度 (米)
    std::string geoid_unit;    // 大地水准面高度单位 (通常为 M)
};

// 将 NMEA 格式的坐标 (DDDMM.MMMM) 转换为十进制度数
double convertNMEAToDecimal(const std::string& nmea_coord, char hemisphere) {
    if (nmea_coord.empty()) {
        std::cerr << "Warning: Empty coordinate string provided.\n";
        return 0.0;
    }

    bool is_latitude = (hemisphere == 'N' || hemisphere == 'S');
    int degrees_length = is_latitude ? 2 : 3;

    // 确保字符串长度足够
    if (nmea_coord.length() < static_cast<size_t>(degrees_length)) {
        std::cerr << "Error: NMEA coordinate string too short: " << nmea_coord << "\n";
        return 0.0;
    }

    // 提取度部分 (前2位或3位)
    std::string degrees_str = nmea_coord.substr(0, degrees_length);
    // 提取分部分 (剩余所有字符，包括小数)
    std::string minutes_str = nmea_coord.substr(degrees_length);

    int degrees = 0;
    double minutes = 0.0;

    try {
        if (!degrees_str.empty()) {
            degrees = std::stoi(degrees_str);
        }
        if (!minutes_str.empty()) {
            minutes = std::stod(minutes_str);
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid number format in coordinate: " << nmea_coord << "\n";
        return 0.0;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range in coordinate: " << nmea_coord << "\n";
        return 0.0;
    }

    // 计算十进制度数
    double decimal_degrees = static_cast<double>(degrees) + (minutes / 60.0);

    // 根据半球调整正负号
    if (hemisphere == 'S' || hemisphere == 'W') {
        decimal_degrees = -decimal_degrees;
    }

    return decimal_degrees;
}

// 计算 NMEA 句子的校验和 (用于快速校验)
unsigned char calculateChecksum(const std::string& sentence_body) {
    unsigned char checksum = 0;
    // sentence_body 应该是 $ 和 * 之间的部分
    for (size_t i = 0; i < sentence_body.length(); ++i) {
        checksum ^= static_cast<unsigned char>(sentence_body[i]);
    }
    return checksum;
}

// 快速校验 NMEA 句子 (在完整解析前调用)
// sentence_part 应该是包含 $...*XX 的部分
bool quickValidateNMEA(const std::string& sentence_part) {
    if (sentence_part.length() < 6 || sentence_part[0] != '$' || sentence_part[sentence_part.length() - 3] != '*') {
        return false;
    }

    // 找到 * 的位置
    size_t asterisk_pos = sentence_part.length() - 3;

    // 提取校验和字符串 (例如 "4B")
    std::string checksum_str = sentence_part.substr(asterisk_pos + 1, 2);

    // 检查校验和是否为有效的十六进制字符
    if (checksum_str.length() != 2 || !std::isxdigit(checksum_str[0]) || !std::isxdigit(checksum_str[1])) {
        return false;
    }

    unsigned char received_checksum = 0;
    try {
        received_checksum = static_cast<unsigned char>(std::stoi(checksum_str, nullptr, 16));
    } catch (const std::exception&) {
        return false; // 转换失败
    }

    // 计算期望的校验和 (不包括 $)
    std::string body_to_check = sentence_part.substr(1, asterisk_pos - 1);
    unsigned char calculated_checksum = calculateChecksum(body_to_check);

    // 比较校验和
    return received_checksum == calculated_checksum;
}

// 检查是否是 GGA 消息类型 (在快速校验后调用)
bool isGGA(const std::string& sentence_part) {
    if (sentence_part.length() < 6 || sentence_part[0] != '$') {
        return false;
    }
    // 找到 * 的位置
    size_t asterisk_pos = sentence_part.find('*');
    if (asterisk_pos == std::string::npos) {
        return false;
    }
    // 提取消息类型字段 (例如 GNGGA, GPGGA)
    std::string msg_type = sentence_part.substr(1, asterisk_pos - 1);
    // 在第一个逗号处截断，获取消息头
    size_t comma_pos = msg_type.find(',');
    if (comma_pos != std::string::npos) {
        msg_type = msg_type.substr(0, comma_pos);
    }
    return (msg_type == "GNGGA" || msg_type == "GPGGA");
}


// 解析单个 GGA 句子 (完整解析)
bool parseGGA(const std::string& sentence, GGAData& data) {
    // 1. 校验和验证 (完整验证)
    size_t asterisk_pos = sentence.find('*');
    if (asterisk_pos == std::string::npos || asterisk_pos + 3 > sentence.length()) { // 至少需要 *XX
        // std::cerr << "Error: No checksum found or incomplete sentence.\n";
        return false; // 没有校验和或句子不完整
    }

    // 提取校验和字符串 (例如 "4B")
    std::string checksum_str = sentence.substr(asterisk_pos + 1, 2);
    unsigned char received_checksum = 0;
    try {
        received_checksum = static_cast<unsigned char>(std::stoi(checksum_str, nullptr, 16));
    } catch (const std::invalid_argument& e) {
        // std::cerr << "Error: Invalid checksum format: " << checksum_str << "\n";
        return false;
    } catch (const std::out_of_range& e) {
        // std::cerr << "Error: Checksum value out of range: " << checksum_str << "\n";
        return false;
    }

    // 计算期望的校验和 (不包括 $)
    std::string body_to_check = sentence.substr(1, asterisk_pos - 1);
    unsigned char calculated_checksum = calculateChecksum(body_to_check);

    // 比较校验和
    if (received_checksum != calculated_checksum) {
        // std::cerr << "Error: Checksum mismatch. Received: 0x" << std::hex << static_cast<int>(received_checksum)
        //           << ", Calculated: 0x" << static_cast<int>(calculated_checksum) << std::dec << "\n";
        return false; // 校验和不匹配
    }

    // 2. 解析数据字段
    // 提取消息体部分用于分割 (去掉开头的$和结尾的*及校验和)
    std::string body = sentence.substr(1, asterisk_pos - 1);
    std::stringstream ss(body);
    std::string field;
    std::vector<std::string> fields;

    // 使用逗号','作为分隔符分割字段
    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }

    // 检查字段数量是否足够 (GGA 至少有14个字段)
    // $GxGGA,1,2,3,4,5,6,7,8,9,10,11,12,13,14*checksum
    if (fields.size() < 14) {
        // std::cerr << "Error: Insufficient fields in GGA sentence. Found: " << fields.size() << "\n";
        return false;
    }

    // 3. 填充 GGAData 结构体
    data.message_type = fields[0]; // GNGGA or GPGGA

    // 检查消息类型 (冗余检查，但更安全)
    if (data.message_type != "GNGGA" && data.message_type != "GPGGA") {
        // std::cerr << "Error: Not a GGA sentence. Type: " << data.message_type << "\n";
        return false;
    }

    data.time_utc = fields[1]; // UTC 时间

    // 纬度 (ddmm.mmmm)
    std::string lat_str = fields[2];
    data.lat_hemisphere = fields[3].empty() ? ' ' : fields[3][0]; // N/S
    if (!lat_str.empty() && data.lat_hemisphere != ' ') {
        data.latitude = convertNMEAToDecimal(lat_str, data.lat_hemisphere);
        if (data.latitude == 0.0 && lat_str != "0000.0000") { // 检查转换是否失败
            return false; // 转换失败已在 convertNMEAToDecimal 内记录
        }
    } else {
        data.latitude = 0.0;
    }

    // 经度 (dddmm.mmmm)
    std::string lon_str = fields[4];
    data.lon_hemisphere = fields[5].empty() ? ' ' : fields[5][0]; // E/W
    if (!lon_str.empty() && data.lon_hemisphere != ' ') {
        data.longitude = convertNMEAToDecimal(lon_str, data.lon_hemisphere);
        if (data.longitude == 0.0 && lon_str != "00000.0000") { // 检查转换是否失败
            return false; // 转换失败已在 convertNMEAToDecimal 内记录
        }
    } else {
        data.longitude = 0.0;
    }

    try {
        data.fix_quality = fields[6].empty() ? 0 : std::stoi(fields[6]);         // 定位质量
        data.num_satellites = fields[7].empty() ? 0 : std::stoi(fields[7]);     // 卫星数
        data.hdop = fields[8].empty() ? 0.0 : std::stod(fields[8]);             // HDOP
        data.altitude = fields[9].empty() ? 0.0 : std::stod(fields[9]);         // 海拔
        data.altitude_unit = fields[10];                                         // 海拔单位
        data.geoid_height = fields[11].empty() ? 0.0 : std::stod(fields[11]);   // 大地水准面高度
        data.geoid_unit = fields[12];                                            // 大地水准面高度单位
        // fields[13] 是差分数据年龄 (通常为空)
        // fields[14] 是差分参考站ID (通常为空)
    } catch (const std::invalid_argument& e) {
        // std::cerr << "Error: Invalid number format in GGA fields.\n";
        return false;
    } catch (const std::out_of_range& e) {
        // std::cerr << "Error: Number out of range in GGA fields.\n";
        return false;
    }

    return true; // 解析成功
}

// 打印 GGA 数据
void printGGAData(const GGAData& data) {
    std::cout << "\n--- Parsed GGA Data ---\n";
    std::cout << "Message Type: " << data.message_type << "\n";
    std::cout << "UTC Time: " << data.time_utc << "\n";
    std::cout << "Latitude: " << std::fixed << std::setprecision(8) << data.latitude << " " << data.lat_hemisphere << "\n";
    std::cout << "Longitude: " << std::fixed << std::setprecision(8) << data.longitude << " " << data.lon_hemisphere << "\n";
    std::cout << "Fix Quality: " << data.fix_quality << "\n";
    std::cout << "Number of Satellites: " << data.num_satellites << "\n";
    std::cout << "HDOP: " << std::fixed << std::setprecision(2) << data.hdop << "\n";
    std::cout << "Altitude: " << std::fixed << std::setprecision(2) << data.altitude << " " << data.altitude_unit << "\n";
    std::cout << "Geoid Height: " << std::fixed << std::setprecision(2) << data.geoid_height << " " << data.geoid_unit << "\n";
    std::cout << "----------------------\n";
}

int main() {
    // *** 在这里指定你的混合二进制/ASCII NMEA 文件路径 ***
    std::string nmea_file_path = R"(D:\work\myproject\work\c_work\NMEA_parser\testlog\test.log)"; // <--- 修改为你的文件路径

    // 以二进制模式打开文件，这对于处理混合数据至关重要
    std::ifstream file(nmea_file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << nmea_file_path << std::endl;
        return 1; // 返回错误码
    }

    const size_t BUFFER_SIZE = 4096; // 读取缓冲区大小
    char buffer[BUFFER_SIZE];
    std::string data_buffer; // 用于累积从文件读取的数据
    bool found_dollar = false;
    std::string potential_sentence;
    int sentence_count = 0;

    std::cout << "Starting to parse mixed binary/ASCII file: " << nmea_file_path << std::endl;

    while (file.good()) {
        // 从文件读取一块数据到缓冲区
        file.read(buffer, BUFFER_SIZE);
        std::streamsize bytes_read = file.gcount(); // 获取实际读取的字节数

        if (bytes_read == 0) {
            break; // 文件读取完毕
        }

        // 将读取到的字节追加到累积缓冲区
        data_buffer.append(buffer, bytes_read);

        // 在累积的数据中查找和处理 NMEA 句子
        size_t pos = 0;
        while (pos < data_buffer.length()) {
            if (!found_dollar) {
                // 寻找句子开始标志 '$'
                size_t dollar_pos = data_buffer.find('$', pos);
                if (dollar_pos != std::string::npos) {
                    found_dollar = true;
                    potential_sentence.clear();
                    potential_sentence += '$'; // 添加找到的 '$'
                    pos = dollar_pos + 1; // 移动到 '$' 之后
                } else {
                    // 当前缓冲区中没有 '$'，丢弃已处理的部分
                    data_buffer.erase(0, pos);
                    pos = 0;
                    break; // 退出内层循环，读取更多数据
                }
            } else {
                // 已经找到 '$'，收集字符直到找到 '*' 和校验和
                while (pos < data_buffer.length()) {
                    char ch = data_buffer[pos];
                    potential_sentence += ch;
                    pos++;

                    if (ch == '*') {
                        // 找到 '*'，检查后面是否至少有两位校验和
                        if (pos + 1 < data_buffer.length()) {
                            // 检查校验和字符是否是十六进制数字
                            bool valid_checksum_chars = true;
                            for(int i = 0; i < 2 && pos + i < data_buffer.length(); ++i) {
                                char csum_ch = data_buffer[pos + i];
                                if (!std::isxdigit(static_cast<unsigned char>(csum_ch))) {
                                    valid_checksum_chars = false;
                                    break;
                                }
                            }
                            if (valid_checksum_chars) {
                                // 添加校验和字符
                                potential_sentence += data_buffer[pos];
                                potential_sentence += data_buffer[pos + 1];
                                pos += 2; // 移动到校验和之后

                                // --- 快速校验和类型检查 ---
                                if (quickValidateNMEA(potential_sentence)) {
                                    if (isGGA(potential_sentence)) {
                                        // --- 寻找句子结束标志 (换行符) ---
                                        // NMEA 句子通常以 \r\n 或 \n 结尾
                                        bool found_end = false;
                                        if (pos < data_buffer.length() && data_buffer[pos] == '\r') {
                                            potential_sentence += '\r';
                                            pos++;
                                        }
                                        if (pos < data_buffer.length() && data_buffer[pos] == '\n') {
                                            potential_sentence += '\n';
                                            pos++;
                                            found_end = true;
                                        }

                                        // 如果找到了完整的句子（包括换行），则进行完整解析
                                        if (found_end) {
                                            // 移除末尾的 \r\n (如果存在) 以获得干净的句子
                                            std::string clean_sentence = potential_sentence;
                                            if (!clean_sentence.empty() && clean_sentence.back() == '\n') {
                                                clean_sentence.pop_back();
                                            }
                                            if (!clean_sentence.empty() && clean_sentence.back() == '\r') {
                                                clean_sentence.pop_back();
                                            }

                                            GGAData gga_data;
                                            if (parseGGA(clean_sentence, gga_data)) {
                                                sentence_count++;
                                                std::cout << "\n--- Found and Parsed GGA Sentence #" << sentence_count << " ---\n";
                                                std::cout << "Raw sentence: " << clean_sentence << std::endl;
                                                printGGAData(gga_data);
                                            } // else 部分的错误信息在 parseGGA 内部被注释了
                                        }
                                        // 如果没找到换行，我们保留 potential_sentence 和 pos 状态，继续收集
                                    } else {
                                        // 不是 GGA 句子，跳过
                                        // std::cout << "Skipping non-GGA sentence: " << potential_sentence << std::endl;
                                    }
                                } else {
                                    // 校验和失败，跳过
                                    // std::cout << "Skipping sentence with failed checksum: " << potential_sentence << std::endl;
                                }

                                // 重置状态，寻找下一个句子
                                found_dollar = false;
                                potential_sentence.clear();
                                break; // 跳出内层 while 循环，继续处理缓冲区剩余部分
                            }
                            // 如果 '*' 后面的字符不是有效的校验和，继续收集
                        }
                        // 如果 '*' 后面没有足够的字符，继续收集
                    }
                }
                // 如果内层循环因为缓冲区用完而退出，保留 potential_sentence 状态，读取更多数据
            }
        }
    }

    // 关闭文件
    file.close();
    std::cout << "\nFinished processing file. Total GGA sentences found and parsed: " << sentence_count << std::endl;
    return 0;
}




#include "CTools.h"
const std::string CTools::SAFE_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789"
"-_.~";
//递归的创建文件目录
bool CTools::createDirectoryRecursive(const std::string& path, mode_t mode)
{
    size_t pos = 0;
    std::string currentDir;

    if (path[0] == '/') {
        currentDir = "/";
        pos = 1;
    }

    while (pos != std::string::npos) {
        pos = path.find('/', pos);
        currentDir = path.substr(0, pos);

        if (!currentDir.empty()) {
            if (mkdir(currentDir.c_str(), mode) != 0) {
                if (errno != EEXIST) { // 忽略已存在的目录
                    perror(("创建目录失败: " + currentDir).c_str());
                    return false;
                }
            }
        }

        if (pos != std::string::npos) {
            pos++;
        }
    }
    return true;
}
/*
函数功能：int转string
函数参数：int num,int minlen 最小长度，即如果转化出来的string的长度小于该长度则需要在前面自动补0
函数返回：string
*/
string CTools::itos(int num, int minlen) {
	stringstream ss;
	ss << num;
	string res = ss.str();
	if (res.size() < minlen)
	{
		res.insert(0, minlen - res.size(), '0');
	}

	return res;
	//写法2	
	//	//使用 std::setw 和 std::setfill 自动补零
	//    ss << std::setw(minlen) << std::setfill('0') << num;
	//    return ss.str();
}

uint32_t CTools::crc32(const uint8_t* data, size_t length)
{
	static const uint32_t polynomial = 0xEDB88320;
	uint32_t crc = 0xFFFFFFFF;
	for (size_t i = 0; i < length; ++i) {
		uint8_t byte = data[i];
		crc ^= byte;
		for (int j = 0; j < 8; ++j) {
			if (crc & 1) {
				crc = (crc >> 1) ^ polynomial;
			}
			else {
				crc >>= 1;
			}
		}

	}
	return crc ^ 0xFFFFFFFF;
}

std::string CTools::urlEncode(const std::string& str) {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex << std::uppercase;

    for (char c : str) {
        // 检查是否是安全字符
        if (SAFE_CHARS.find(c) != std::string::npos) {
            encoded << c;
        }
        // 处理空格（特殊规则：编码为+或%20，这里采用%20更通用）
        else if (c == ' ') {
            encoded << "%20";
        }
        // 其他字符：百分号编码
        else {
            encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return encoded.str();
}

std::string CTools::urlDecode(const std::string& str) {
    std::ostringstream decoded;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            // 检查是否有足够的字符用于解码
            if (i + 2 < str.length()) {
                int value;
                std::istringstream hexStream(str.substr(i + 1, 2));
                hexStream >> std::hex >> value;
                decoded << static_cast<char>(value);
                i += 2; // 跳过已处理的两位十六进制数
            }
            else {
                // 无效编码，保留原始字符
                decoded << str[i];
            }
        }
        // 处理空格特殊情况（+号表示空格）
        else if (str[i] == '+') {
            decoded << ' ';
        }
        else {
            decoded << str[i];
        }
    }
    return decoded.str();
}
//生成随机验证码
std::string CTools::generateCode(int length) {
    static const char digits[] = "0123456789";
    std::string code;

    for (int i = 0; i < length; ++i) {
        code += digits[rand() % 10];
    }

    return code;
}
//时间戳转时间字符串
string CTools::convertTimeStamp2TimeStr(time_t timeStamp) 
{
    struct tm* timeinfo = nullptr;
    char buffer[80];
    timeinfo = localtime(&timeStamp);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}
//时间字符串转时间戳
time_t CTools::convertTimeStr2TimeStamp(string timeStr)
{
    struct tm timeinfo;
    strptime(timeStr.c_str(), "%Y-%m-%d %H:%M:%S", &timeinfo);
    time_t tt;
    tt = mktime(&timeinfo);
    return tt;
}
/*
函数功能：获取日期时间"%Y-%m-%d %H:%M:%S"
函数参数：string format，获取的日期时间格式"%Y-%m-%d %H:%M:%S"
函数返回：string 返回给定的格式所对应的当前时间字符串
*/
string CTools::getDatetime(string format) {
    //获取日期
    // 获取当前时间的时间戳
    time_t currentTime = time(NULL);

    // 将时间戳转换为本地时间
    tm* localTime = localtime(&currentTime);
    // 格式化输出时间的缓冲区
    char buffer[100];

    // 将时间格式化为字符串
    // 格式：年-月-日 时:分:秒
    strftime(buffer, sizeof(buffer), format.c_str(), localTime);
    return string(buffer);
}
#include "Logger.h"
#include <fstream>
#include <iostream>

Logger::Logger(const std::string& filename) : filename_(filename) {}

void Logger::log(const std::string& line) {
    std::ofstream out(filename_, std::ios::app);
    if (!out) {
        // 打不开（比如当前目录只读、或没有写权限）时不能静默失败：
        // 否则玩家玩完一整局，日志一个字都没有，还查不出原因。
        // 只提醒一次，不然后面每一行都刷屏。
        static bool warned = false;
        if (!warned) {
            warned = true;
            std::cerr << "警告：日志文件打不开 —— " << filename_
                      << "（当前目录可能不可写；请在 exe 所在目录运行）\n";
        }
        return;
    }
    out << line << "\n";
}

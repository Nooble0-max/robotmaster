#pragma once
#include <string>

class Logger {
public:
    Logger(const std::string& filename);
    void log(const std::string& line);

private:
    std::string filename_;
};

#include "Input.h"
#include "Robot.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace {

// 首尾空白一律去掉：用户手滑多敲一个空格，不该变成"看不懂这条指令"。
std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    const size_t b = s.find_first_not_of(ws);
    if (b == std::string::npos) {
        return "";
    }
    const size_t e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

}   // namespace

void Input::prompt(const Robot& r) const {
    cout << "\n  " << r.name()
         << "  HP " << r.hp() << '/' << r.maxHp()
         << "    命令: move/attack/repair 行 列 | supply | wait | help | quit\n  > ";
    cout.flush();      // 不让提示符卡在缓冲区里，用户才知道该敲字了
}

std::string Input::readLine() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return "quit";     // 输入流结束（Ctrl+Z / 管道读完）就当玩家退出，别死循环
    }
    return trim(line);
}

Command Input::parse(const std::string& line) {
    std::istringstream in(line);
    std::string word;
    if (!(in >> word)) {
        return Command{};      // 空行 → Invalid（kind 的默认值）
    }

    Command cmd;
    if (word == "move" || word == "m" || word == "移动") {
        cmd.kind = Command::Kind::Move;
    } else if (word == "attack" || word == "a" || word == "攻击") {
        cmd.kind = Command::Kind::Attack;
    } else if (word == "repair" || word == "r" || word == "修理") {
        cmd.kind = Command::Kind::Repair;
    } else if (word == "supply" || word == "s" || word == "补给") {
        cmd.kind = Command::Kind::Supply;
    } else if (word == "wait" || word == "w" || word == "pass" || word == "待机") {
        cmd.kind = Command::Kind::Wait;
    } else if (word == "help" || word == "h" || word == "帮助") {
        cmd.kind = Command::Kind::Help;
    } else if (word == "quit" || word == "q" || word == "退出") {
        cmd.kind = Command::Kind::Quit;
    } else {
        return cmd;            // 认不出的词：保持 Invalid，让 Controller 提示重输
    }

    // 三个行动命令必须带坐标（行 列）。少写一个就当作没看懂，
    // 不要拿默认的 {-1,-1} 去调 Game——那样虽然也会被否决，
    // 但错在"输入残缺"，提示要准确。
    if (cmd.kind == Command::Kind::Move ||
        cmd.kind == Command::Kind::Attack ||
        cmd.kind == Command::Kind::Repair) {
        if (!(in >> cmd.target.r >> cmd.target.c)) {
            cmd.kind = Command::Kind::Invalid;
        }
    }
    return cmd;
}

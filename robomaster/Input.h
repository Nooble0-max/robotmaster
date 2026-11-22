#pragma once
#include <string>
#include "common.h"

class Robot;

// 一条玩家指令。
// 注意它只表示"玩家想干什么"，不代表这一步合法——
// 合不合法由 Game 判断，这里绝不替裁判下结论。
struct Command {
    enum class Kind { Move, Attack, Repair, Supply, Wait, Help, Quit, Invalid };
    Kind kind = Kind::Invalid;
    Pos  target{-1, -1};      // 只有 Move / Attack / Repair 用得到
};

// ============================================================
//  Input：视图层的"读"——把键盘上的一行字翻译成一条 Command。
//
//  它只懂"文字"，不懂"规则"：不知道射程、不知道谁该动、
//  也不知道 move 到那格行不行。翻译完就交差，判断是 Controller 和 Game 的事。
//  这样拆的好处：将来把控制台换成鼠标点击，只需要换掉这个类，
//  parse 出来的 Command 和后面的流程一模一样。
// ============================================================
class Input {
public:
    void prompt(const Robot& r) const;              // 打印"轮到谁 + 怎么输入"
    std::string readLine();                         // 读一行，去掉首尾空白
    static Command parse(const std::string& line);  // 文本 → 指令
};

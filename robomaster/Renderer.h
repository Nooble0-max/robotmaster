#pragma once
#include <string>
#include "common.h"

class Game;
class Robot;
class Base;

// ============================================================
//  Renderer：视图层的"转播"——只把 Game 的状态画成人能看的样子。
//
//  两条铁律（Step 6 全部靠它守住模型 / 视图分离）：
//   1) 只读：只用 Game 的 const 接口（map / robots / redBase …），
//      一个状态都不改。想改战场只有一条路——回去调 Game 的行动函数。
//   2) 不裁决：这里不判断"这一步合不合法"，也不知道谁能动、能打多远。
//      Renderer 只负责"画出来"，合法性全在 Game 那道闸里。
//
//  现在先做控制台版：画 7×7 字符棋盘 + 右侧小面板 + 双方基地 + 每台机器的血量弹药。
//  将来上 EasyX 时，只要再写一个 Renderer 实现，模型层一行都不用改——
//  这就是当初坚持分层的回报。
//
//  ---- 棋盘符号表（要改符号，只改 Renderer.cpp 的 cellChar / robotChar）----
//    .  空地            #  障碍            +  补给
//    R  红方基地        B  蓝方基地
//    I  步兵            S  大狙            E  工程
//        ↑ 大写 = 红方   ↑ 小写 = 蓝方     （I 红 / i 蓝，S 红 / s 蓝，E 红 / e 蓝）
//
//  血量 / 弹药不画在格子里，而是画在棋盘下面的清单和右侧小面板里，
//  每帧从 Robot / Base 重新读一次——所以画面上的数字永远是当前值。
//  受伤的机器：面板里标 !（控制台支持颜色时还会标亮红），阵亡的变灰。
// ============================================================

// 一次行动的结果摘要：Controller 在调用 Game 之前/之后读数字填好，
// 再交给 Renderer 印成一行中文战报。
// 它只描述"发生了什么"，不含任何规则判断；数字全是行动前后从模型读的差值，
// 视图层不自己算伤害（伤害多少是 Robot::damage() 决定的，那是模型的事）。
struct ActionReport {
    enum class Kind { Move, Attack, Repair, Supply, Wait };

    Kind kind = Kind::Wait;
    const Robot* actor = nullptr;       // 行动的那台

    Pos from{-1, -1};                   // Move：起点
    Pos to{-1, -1};                     // Move：终点

    const Robot* targetRobot = nullptr; // Attack / Repair：目标机器人
    const Base*  targetBase  = nullptr; // Attack：目标是基地
    int hpBefore = 0;                   // 目标行动前 HP
    int hpAfter  = 0;                   // 目标行动后 HP
    int ammoBefore = 0;                 // Supply：补给前弹药
    int ammoAfter  = 0;                 // Supply：补给后弹药
};

class Renderer {
public:
    Renderer();                               // 切 UTF-8、按需打开 ANSI 颜色
    void draw(const Game& game) const;        // 画一帧：回合信息 + 棋盘 + 面板 + 状态
    void reportAction(const ActionReport& rep) const;  // 印一行战报（移动/攻击/修理）
    void drawResult(const Game& game) const;  // 终局画面（只在 over() 为真时调用）

private:
    char cellChar(const Game& game, Pos p) const;  // 某一格该画什么字符
    static char robotChar(const Robot& r);         // 机器人 → 棋盘符号

    std::string cellText(const Game& game, Pos p) const;      // 带颜色的格子
    std::string panelCell(const Game& game, Team t, int index) const;  // 面板里的一格
    std::string targetLabel(const ActionReport& rep) const;   // "蓝 工程" / "蓝基地"
    std::string paint(const std::string& text, Team team,
                      bool damaged, bool dead) const;         // 套 ANSI 颜色

    bool color_;   // 终端支持 ANSI 颜色吗（输出被重定向到文件时自动关掉）
};

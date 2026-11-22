#include "Renderer.h"
#include "Game.h"
#include "Map.h"
#include "Base.h"
#include "Robot.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <cstdio>
#include <unistd.h>
#endif

using namespace std;

namespace {

// 阵营怎么称呼，是"显示"问题，所以放在视图层；模型层不管叫"红方"还是"Red"。
const char* teamName(Team t) {
    return t == Team::Red ? "红方" : "蓝方";
}

// Pos 没有 operator==，视图里只做一次比较，不值得为它加运算符重载。
bool samePos(Pos a, Pos b) {
    return a.r == b.r && a.c == b.c;
}

// 右侧小面板的格子宽度：'!'或空格 + 兵种字母 + 空格 + 血(3) + '/' + 弹(2) = 9
constexpr int kPanelCell = 9;

}   // namespace

Renderer::Renderer() {
    color_ = false;
#ifdef _WIN32
    // MSVC 用 /utf-8 编译，源码里的汉字是 UTF-8 字节；
    // 但控制台默认还是 GBK（代码页 936），两边对不上就会印出乱码。
    // 这里把控制台的输入 / 输出都切到 UTF-8。
    // 属于 Windows 控制台的历史包袱，和模型 / 视图分层无关；
    // 以后换成 EasyX 窗口，这些代码可以直接删掉。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // ANSI 颜色：只有"真的连着控制台"才开。
    // Windows 10 以后的控制台要显式打开 VT 处理，否则 \x1b[31m 会变成乱码字；
    // 输出被重定向到文件时 GetConsoleMode 会失败，颜色自动关掉，文件里干干净净。
    const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        if (SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            color_ = true;
        }
    }
#else
    color_ = isatty(fileno(stdout)) != 0;
#endif
}

// ---------------- 颜色 ----------------

// 套一层 ANSI 颜色。注意转义码不占显示宽度，所以对齐不会乱。
//   阵亡 = 灰    受伤 = 亮红加粗    正常 = 本方颜色（红方暗红 / 蓝方亮蓝）
std::string Renderer::paint(const std::string& text, Team team,
                            bool damaged, bool dead) const {
    if (!color_) {
        return text;
    }
    const char* code = "\x1b[0m";
    if (dead) {
        code = "\x1b[90m";
    } else if (damaged) {
        code = "\x1b[1;91m";
    } else if (team == Team::Red) {
        code = "\x1b[31m";
    } else {
        code = "\x1b[94m";
    }
    return std::string(code) + text + "\x1b[0m";
}

// ---------------- 单个格子 ----------------

// 某一格画什么：先看有没有机器人（占位优先），再问地形。
// 注意这里只"问"不"判"：isEmpty / isObstacle 都是 Map 的查询，
// 不存在"能不能走"这种规则判断。
char Renderer::cellChar(const Game& game, Pos p) const {
    const Map& m = game.map();

    const Robot* r = m.at(p);
    if (r != nullptr) {
        return robotChar(*r);
    }
    if (m.isObstacle(p)) {
        return '#';
    }
    if (m.isSupply(p)) {
        return '+';
    }
    if (m.isBase(p)) {
        return samePos(p, game.redBase().pos()) ? 'R' : 'B';
    }
    return '.';
}

// 格子 + 颜色：机器人按阵营上色，受伤的标亮红（格子里放不下 '!'，用颜色代替）。
std::string Renderer::cellText(const Game& game, Pos p) const {
    const char ch = cellChar(game, p);
    const Robot* r = game.map().at(p);
    if (r == nullptr) {
        return std::string(1, ch);
    }
    return paint(std::string(1, ch), r->team(), r->hp() < r->maxHp(), false);
}

// 机器人怎么画：兵种用一个字母，阵营用大小写区分。
//   兵种    名字符号   红方   蓝方
//   步兵    I          I      i     Infantry
//   大狙    S          S      s     Sniper
//   工程    E          E      e     Engineer
// 名字 → 符号的映射是纯显示问题，所以留在视图层，模型层不需要
// 为了"怎么画"多开一个虚函数。（Step 8 若改兵种名字，只需同步改这张表。）
char Renderer::robotChar(const Robot& r) {
    char c = '?';
    if (r.name() == "步兵") {
        c = 'I';          // Infantry
    } else if (r.name() == "大狙") {
        c = 'S';          // Sniper
    } else if (r.name() == "工程") {
        c = 'E';          // Engineer
    }

    // 红方大写、蓝方小写：同一张棋盘上一眼分敌我。
    if (r.team() == Team::Blue && c != '?') {
        c = static_cast<char>(c - 'A' + 'a');
    }
    return c;
}

// ---------------- 右侧小面板 ----------------

// 面板一格： "!I 100/20" / " S  80/10" / 阵亡时 " e   0/--"
// 固定 9 个字符宽（不含颜色转义码），这样左右两列始终对得齐。
std::string Renderer::panelCell(const Game& game, Team t, int index) const {
    int seen = 0;
    for (const Robot* r : game.robots()) {
        if (r->team() != t) {
            continue;
        }
        if (seen != index) {
            ++seen;
            continue;
        }

        const bool dead = !r->alive();
        const bool hurt = !dead && r->hp() < r->maxHp();

        std::ostringstream out;
        if (dead) {
            out << ' ' << robotChar(*r) << "   0/--";
        } else {
            out << (hurt ? '!' : ' ')
                << robotChar(*r) << ' '
                << std::setw(3) << r->hp() << '/'
                << std::setw(2) << r->ammo();
        }
        return paint(out.str(), r->team(), hurt, dead);
    }
    return std::string(kPanelCell, ' ');   // 这个阵营没有第 index 台
}

// ---------------- 画一帧 ----------------

void Renderer::draw(const Game& game) const {
    cout << '\n';
    cout << "================ 第 " << game.turn() << '/' << game.maxHalfTurns()
         << " 回合 · " << teamName(game.currentTeam()) << "行动 ================\n";

    // 右侧小面板：红方第 i 台 | 蓝方第 i 台，按行先算好
    int redCount = 0;
    int blueCount = 0;
    for (const Robot* r : game.robots()) {
        (r->team() == Team::Red ? redCount : blueCount)++;
    }
    const int rows = redCount > blueCount ? redCount : blueCount;

    vector<string> panel;
    for (int i = 0; i < rows; ++i) {
        panel.push_back(panelCell(game, Team::Red, i) + "   " +
                        panelCell(game, Team::Blue, i));
    }

    // 列号 + 面板表头（面板从第 21 列开始，9 宽一格，中间 3 空格）
    cout << "     ";
    for (int c = 0; c < N; ++c) {
        cout << ' ' << c;
    }
    cout << "  " << "红方     " << "   " << "蓝方\n";

    for (int r = 0; r < N; ++r) {
        cout << "  " << r << "  ";
        for (int c = 0; c < N; ++c) {
            cout << ' ' << cellText(game, Pos{r, c});
        }
        if (r < rows) {
            cout << "  " << panel[r];
        }
        cout << '\n';
    }

    cout << "\n  图例  I/S/E 步兵·大狙·工程（大写红方，小写蓝方）"
            "  # 障碍  + 补给  R/B 基地\n";
    cout << "        右面板：符号 血量/弹药；! 受伤（红字）、阵亡（灰字）\n";

    cout << "  基地  红 " << game.redBase().hp() << '/' << game.redBase().maxHp()
         << "    蓝 " << game.blueBase().hp() << '/' << game.blueBase().maxHp()
         << '\n';

    // 每台机器一行：血量 / 弹药 / 位置都现读现画。
    //   hp() / ammo() 从 Robot 读（被打、被修、扣弹后立刻变）；
    //   位置从 Map 查（位置的唯一权威在 Map，Robot 自己不存）。
    // 这个函数每次行动后都会被重画一遍，所以清单里的数字就是实时的。
    for (Team t : {Team::Red, Team::Blue}) {
        cout << "  " << teamName(t) << '\n';
        for (const Robot* r : game.robots()) {
            if (r->team() != t) {
                continue;
            }
            const bool dead = !r->alive();
            const bool hurt = !dead && r->hp() < r->maxHp();

            cout << "    " << paint(std::string(1, robotChar(*r)),
                                    r->team(), hurt, dead)
                 << ' ' << r->name();
            if (dead) {
                cout << "  阵亡\n";
                continue;
            }
            cout << "  HP " << r->hp() << '/' << r->maxHp()
                 << (hurt ? " !" : "  ")
                 << "  弹 " << r->ammo() << '/' << r->maxAmmo();

            const Pos p = game.map().find(r);
            if (p.r >= 0) {              // 活着却不在图上，说明模型有问题，别硬画
                cout << "  @(" << p.r << ',' << p.c << ')';
            }
            cout << '\n';
        }
    }
}

// ---------------- 战报 ----------------

// 目标怎么称呼：机器人给"蓝 工程"，基地给"蓝基地"。
std::string Renderer::targetLabel(const ActionReport& rep) const {
    if (rep.targetRobot != nullptr) {
        return std::string(teamName(rep.targetRobot->team())) + " " +
               rep.targetRobot->name();
    }
    if (rep.targetBase != nullptr) {
        return std::string(teamName(rep.targetBase->team())) + "基地";
    }
    return "目标";
}

// 一行战报：数字全部来自 ActionReport（行动前后读的真值），
// Renderer 只负责拼中文，不参与任何计算或判断。
void Renderer::reportAction(const ActionReport& rep) const {
    if (rep.actor == nullptr) {
        return;
    }

    cout << "  >> " << teamName(rep.actor->team()) << ' ' << rep.actor->name() << ' ';

    switch (rep.kind) {
    case ActionReport::Kind::Move:
        cout << "移动 (" << rep.from.r << ',' << rep.from.c << ") → ("
             << rep.to.r << ',' << rep.to.c << ")\n";
        break;

    case ActionReport::Kind::Attack:
        cout << "攻击 " << targetLabel(rep)
             << " -" << (rep.hpBefore - rep.hpAfter)
             << "  (" << rep.hpBefore << "→" << rep.hpAfter << ')';
        if (rep.hpAfter <= 0) {
            cout << "，击毁";
        }
        cout << '\n';
        break;

    case ActionReport::Kind::Repair:
        cout << "修理 " << targetLabel(rep)
             << " +" << (rep.hpAfter - rep.hpBefore)
             << "  (" << rep.hpBefore << "→" << rep.hpAfter << ")\n";
        break;

    case ActionReport::Kind::Supply:
        cout << "补给  +" << (rep.hpAfter - rep.hpBefore) << "HP  +"
             << (rep.ammoAfter - rep.ammoBefore) << "弹"
             << "  (HP " << rep.hpBefore << "→" << rep.hpAfter
             << "，弹 " << rep.ammoBefore << "→" << rep.ammoAfter << ")\n";
        break;

    case ActionReport::Kind::Wait:
        return;      // 待机不刷屏
    }
}

void Renderer::drawResult(const Game& game) const {
    cout << "\n================ 对局结束 ================\n";
    cout << "  红基地 " << game.redBase().hp() << '/' << game.redBase().maxHp()
         << "    蓝基地 " << game.blueBase().hp() << '/' << game.blueBase().maxHp()
         << '\n';

    if (!game.redBase().alive() || !game.blueBase().alive()) {
        cout << "  结束原因：基地被摧毁\n";
    } else {
        cout << "  结束原因：到达回合上限，比基地剩余 HP\n";
    }

    // 平局要先判：winner() 在双方都活着且血量相同时给的是兜底值，不能直接用。
    if (game.isDraw()) {
        cout << "  结果：平局\n";
        return;
    }
    // 基地被摧毁时 winner() 直接给答案；到回合上限时要比基地血量。
    const bool baseDown = !game.redBase().alive() || !game.blueBase().alive();
    cout << "  胜者：" << teamName(baseDown ? game.winner() : game.winnerByHp()) << "\n";
}

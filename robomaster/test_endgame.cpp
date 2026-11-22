#include <iostream>
#include "Game.h"
using namespace std;

void show(const char* label, bool v, bool expect) {
    cout << label << " = " << v << " (期望" << expect << ")"
         << (v == expect ? "  OK" : "  <<< 错!") << endl;
}

int main() {
    Game g;

    // 找红步兵、红工程
    Robot* ri = nullptr;
    Robot* re = nullptr;
    for (Robot* r : g.robots()) {
        if (r->team() == Team::Red && r->name() == "步兵") ri = r;
        if (r->team() == Team::Red && r->name() == "工程") re = r;
    }

    // ① 开局没结束
    show("开局 over()", g.over(), false);

    // ② 打自己家基地 → 拒绝（红工程在 {1,3}，红基地 {0,3}）
    show("打自己基地", g.attack(re, {0,3}), false);

    // ③ 打太远的敌方基地 → 拒绝（红步兵 {1,2} 到蓝基地 {6,3} 距离5）
    show("打太远的蓝基地", g.attack(ri, {6,3}), false);

    // ④ 走位：把红步兵从 {1,2} 沿左列绕到 {6,2}（蓝基地 {6,3} 隔壁）
    //     全程红方回合，每次移动都不超过 moveRange=2
    g.move(ri, {1,0});
    g.move(ri, {3,0});
    g.move(ri, {5,0});
    g.move(ri, {6,1});
    g.move(ri, {6,2});
    Pos p = g.map().find(ri);
    cout << "红步兵走位后 = {" << p.r << "," << p.c << "} (期望{6,2})" << endl;

    // ⑤ 贴脸打蓝基地一下，看掉血
    int hp0 = g.blueBase().hp();
    show("贴脸打蓝基地", g.attack(ri, {6,3}), true);
    cout << "  蓝基地 hp " << hp0 << " -> " << g.blueBase().hp()
         << " (期望" << hp0 - 20 << ")" << endl;

    // ⑥ 连续打，直到基地被打烂
    int hits = 0;
    while (!g.over()) {
        g.attack(ri, {6,3});
        hits++;
    }
    cout << "  补刀到基地死，共 " << hits << " 次" << endl;
    show("over() 变 true", g.over(), true);
    cout << "  winner() = " << (g.winner() == Team::Red ? "红" : "蓝")
         << " (期望红)" << endl;

    return 0;
}

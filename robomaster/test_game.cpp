#include <iostream>
#include "Game.h"
using namespace std;

// 打印一个 bool 结果，并和期望值对比
void show(const char* label, bool v, bool expect) {
    cout << label << " = " << v << " (期望" << expect << ")"
         << (v == expect ? "  OK" : "  <<< 错!") << endl;
}

int main() {
    Game g;

    // 找出红步兵、蓝步兵
    Robot* ri = nullptr;
    Robot* bi = nullptr;
    for (Robot* r : g.robots()) {
        if (r->team() == Team::Red  && r->name() == "步兵") ri = r;
        if (r->team() == Team::Blue && r->name() == "步兵") bi = r;
    }

    Pos p = g.map().find(ri);
    cout << "红步兵初始位置 = {" << p.r << "," << p.c << "} (期望{1,2})" << endl;

    // ① 合法移动：{1,2} -> {0,1}，距离2 = 移动上限
    show("合法移动到 {0,1}", g.move(ri, {0,1}), true);

    // ② 走太远：{0,1} -> {0,5}，距离4 > 2
    show("走到 {0,5}(太远)", g.move(ri, {0,5}), false);

    // ③ 基地不能进：{0,1} -> {0,3}，距离2 但 {0,3} 是红基地
    show("走到基地 {0,3}", g.move(ri, {0,3}), false);

    // ④ 红方回合操作蓝方机器人
    show("红方回合动蓝步兵", g.move(bi, {5,1}), false);

    // ⑤ 死人不能动
    ri->takeDamage(1000);
    show("红步兵死后移动", g.move(ri, {1,1}), false);

    // ⑥ endTurn 换边，蓝方回合蓝步兵能动了
    g.endTurn();
    cout << "endTurn 后 currentTeam = " << (g.currentTeam() == Team::Red ? "红" : "蓝")
         << " (期望蓝), turn = " << g.turn() << " (期望1)" << endl;
    show("蓝方回合动蓝步兵", g.move(bi, {5,1}), true);

    return 0;
}

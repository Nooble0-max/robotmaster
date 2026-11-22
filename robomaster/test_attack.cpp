#include <iostream>
#include "Game.h"
using namespace std;

// 打印一个 bool 结果，并和期望值对比
void show(const char* label, bool v, bool expect) {
    cout << label << " = " << v << " (期望" << expect << ")"
         << (v == expect ? "  OK" : "  <<< 错!") << endl;
}

int main() {
    // ================= 一、attack 测试 =================
    Game g;
    Robot* ri = nullptr;   // 红步兵
    Robot* re = nullptr;   // 红工程
    Robot* bi = nullptr;   // 蓝步兵
    for (Robot* r : g.robots()) {
        if (r->team() == Team::Red  && r->name() == "步兵") ri = r;
        if (r->team() == Team::Red  && r->name() == "工程") re = r;
        if (r->team() == Team::Blue && r->name() == "步兵") bi = r;
    }

    // ① 打射程内的空格子（关键：判空要挡住，不能崩）
    show("打空格 {0,2}", g.attack(ri, {0,2}), false);

    // ② 打队友（re 在 {1,3}，红步兵贴脸打同队）
    show("打队友", g.attack(ri, {1,3}), false);

    // ③ 打太远（{5,2} 距离4 > 射程1）
    show("打太远", g.attack(ri, {5,2}), false);

    // ④ 红方回合用蓝步兵打（阵营闸）
    show("红方回合用蓝步兵打", g.attack(bi, {4,2}), false);

    // ---- 摆一个合法攻击：红步兵下到 {3,2}，蓝步兵上到 {4,2}，贴脸 ----
    g.move(ri, {3,2});   // {1,2} -> {3,2}
    g.endTurn();         // 蓝方回合
    g.move(bi, {4,2});   // {5,2} -> {4,2}
    g.endTurn();         // 红方回合

    int hpBefore = bi->hp();
    show("合法攻击", g.attack(ri, {4,2}), true);
    cout << "  蓝步兵 hp " << hpBefore << " -> " << bi->hp() << " (期望80)" << endl;

    // ---- 打死并清场 ----
    bi->takeDamage(60);   // 80 -> 20，制造"下一击必死"
    show("最后一击", g.attack(ri, {4,2}), true);
    cout << "  蓝步兵 alive = " << bi->alive() << " (期望0)" << endl;
    cout << "  蓝步兵那格 at({4,2}) = "
         << (g.map().at({4,2}) == nullptr ? "空" : "有人") << " (期望空)" << endl;

    // ================= 二、repair 测试 =================
    Game g2;
    Robot* r2i = nullptr;   // 红步兵
    Robot* r2e = nullptr;   // 红工程
    Robot* r2s = nullptr;   // 红狙击
    Robot* b2e = nullptr;   // 蓝工程
    for (Robot* r : g2.robots()) {
        if (r->team() == Team::Red  && r->name() == "步兵") r2i = r;
        if (r->team() == Team::Red  && r->name() == "工程") r2e = r;
        if (r->team() == Team::Red  && r->name() == "大狙") r2s = r;
        if (r->team() == Team::Blue && r->name() == "工程") b2e = r;
    }

    // ① 修空格（判空挡住）
    show("修空格 {0,2}", g2.repair(r2e, {0,2}), false);

    // ② 修满血的（狙击 80/80）
    show("修满血的", g2.repair(r2e, {1,4}), false);

    // ③ 步兵不会修（canRepair 闸）
    show("步兵不能修", g2.repair(r2i, {1,3}), false);

    // ④ 红方回合用蓝工程修（阵营闸）
    show("红方回合用蓝工程修", g2.repair(b2e, {5,2}), false);

    // ⑤ 正常修：红步兵打伤到 70，红工程贴脸修 -> 80
    r2i->takeDamage(30);
    show("工程修受伤队友", g2.repair(r2e, {1,2}), true);
    cout << "  红步兵 hp = " << r2i->hp() << " (期望80)" << endl;

    // ⑥ 封顶：狙击打到 75，工程修一次 -> 应 clamp 到 80，不是 85
    r2s->takeDamage(5);
    show("修到封顶", g2.repair(r2e, {1,4}), true);
    cout << "  红狙击 hp = " << r2s->hp() << " (期望80，不是85)" << endl;

    return 0;
}

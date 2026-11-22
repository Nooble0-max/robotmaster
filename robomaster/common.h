#pragma once
#include <cstdlib>
using namespace std;


enum class Team { Red, Blue };

// 地图坐标
struct Pos {
    int r;
    int c;
};

// 攻击射程、移动范围用曼哈顿距离
inline int manhattan(const Pos& a, const Pos& b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
}

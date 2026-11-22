#pragma once
#include "common.h"

// 格子地形类型
enum class Terrain { Empty, Obstacle, Supply, Base };

class Robot;   // 前向声明：Map 里只存 Robot 指针，不用看到 Robot 完整定义

constexpr int N = 7;   // 地图边长（待定，先 7×7）

// ============================================================
//  Map：战场。负责两件事——
//   1) 地形：哪格是障碍 / 补给 / 基地；
//   2) 占位：哪台机器人站在哪格（全项目唯一的"位置"权威来源）。
//
//  注意分工：Map 只回答"这格是什么 / 谁站这"，不判断"能不能走 / 能不能打"。
//  规则判断（合法性）属于 Game。这样规则只有一道闸，不会绕过。
// ============================================================
class Map {
public:
    Map();   // 初始全是空地，之后由 Game 调 setTerrain 布置

    // ---- 布置地形（开赛前调用一次）----
    void setTerrain(Pos p, Terrain t);

    // ---- 查询（Game 做合法性判断时全用这些）----
    bool inBounds(Pos p) const;     // 越界吗
    bool isObstacle(Pos p) const;   // 障碍吗
    bool isSupply(Pos p) const;     // 补给区吗
    bool isBase(Pos p) const;       // 基地吗
    bool isOccupied(Pos p) const;   // 有机器人占着吗
    bool isEmpty(Pos p) const;      // 能走进去吗（界内、非障碍、非基地、无人占；补给区可进）

    // ---- 机器人占位 ----
    void place(Robot* r, Pos p);     // 把机器人放到 p（开局布置，假设 p 是空位）
    Pos  find(const Robot* r) const; // 查机器人现在在哪；找不到返回 {-1,-1}
    Robot* at(Pos p) const;          // 查 p 这格站的是谁；没人返回 nullptr
    void move(Robot* r, Pos to);     // 把机器人挪到 to（假设 to 已合法且为空，Game 查过）
    void remove(Robot* r);           // 把机器人从地图上清掉（死亡时用）

private:
    Terrain grid[N][N];   
    Robot*  robot[N][N];   
};

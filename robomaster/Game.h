#pragma once
#include "common.h"
#include "Map.h"
#include "Base.h"
#include "Robot.h"
#include "Logger.h"
#include "config.h"
#include <vector>

// ============================================================
//  Game：裁判（模型层唯一的"规则"所在）。
//  职责三件事：
//   1) 合法性判断：这个行动能不能做（阵营、死活、地形、范围）；
//   2) 结算：行动后扣血 / 死亡 / 补给 / 修理；
//   3) 终局：基地被毁 → 谁赢。
//
//  注意分工：Game 用 Map 的查询（inBounds / isEmpty / at…）拼出规则，
//  但 Map 自己不含规则。这样规则只有一道闸，全在 Game。
// ============================================================
class Game {
public:
    Game();    // 布置地图、基地、机器人；红方先手
    ~Game();   // Game 拥有机器人，负责 delete

    // ---- 查询（视图层 / 测试用，只读）----
    const Map&  map()       const { return map_; }
    const Base& redBase()   const { return redBase_; }
    const Base& blueBase()  const { return blueBase_; }
    const std::vector<Robot*>& robots() const { return robots_; }
    Team currentTeam() const { return currentTeam_; }
    int  turn()        const { return turn_; }
    int  maxHalfTurns() const { return cfg::kMaxHalfTurns; }   // 新增（Step 8）

    // ---- 行动：判断合法性，合法就执行并返回 true，否则返回 false ----
    bool move(Robot* r, Pos to);
    bool attack(Robot* r, Pos target);
    bool repair(Robot* r, Pos target);   // 只有工程能修
    bool supply(Robot* r);               // 新增（Step 8）：补给，须站在补给区

    // ---- 回合 ----
    void endTurn();   // 换边，回合数 +1

    // ---- 终局 ----
    bool over() const;    // 任一基地被摧毁
    Team winner() const;  // 只在 over() 为真时问
    bool isDraw() const;      // 新增（Step 8）：到上限且两边基地血量相同
    Team winnerByHp() const;  // 新增（Step 8）：到上限时比基地 HP（平局先查 isDraw）
    void logFinal();          // 新增：把最终结果写进对局记录
private:
    Map  map_;
    Base redBase_;
    Base blueBase_;
    std::vector<Robot*> robots_;   // 全部机器人（红蓝都有）
    Team currentTeam_;
    int  turn_;
    Logger logger_;
};

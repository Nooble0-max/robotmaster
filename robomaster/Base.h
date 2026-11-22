#pragma once
#include "common.h"


//  Base：基地。

class Base {
public:
    Base(Team team, Pos pos, int maxHp);

    // 通用查询：直接写死
    Team team()  const { return team_; }
    Pos  pos()   const { return pos_; }
    int  hp()    const { return hp_; }
    int  maxHp() const { return maxHp_; }
    bool alive() const { return hp_ > 0; }

    void takeDamage(int dmg);   // 扣血

private:
    Team team_;
    Pos  pos_;      // 基地固定位置
    int  hp_;
    int  maxHp_;
};

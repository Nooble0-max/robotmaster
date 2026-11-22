#pragma once
#include <string>
#include "common.h"
using namespace std;


class Robot {
public:
    Robot(Team team, const string& name, int maxHp);

    // 虚析构：将来用 `vector<Robot*>` 存子类、delete 基类指针时，
    // 才能正确地析构到子类。基类给个空实现即可。
    virtual ~Robot() {}

    // ---- 通用查询：三种兵都一样，所以直接写死，不用 virtual ----
    Team   team()  const { return team_; }
    string name()  const { return name_; }
    int    hp()    const { return hp_; }
    int    maxHp() const { return maxHp_; }
    int    ammo()  const { return ammo_; }
    bool   alive() const { return hp_ > 0; }

    // ---- 状态修改：结算只能走这两个口子，不许外界直接改成员 ----
    void takeDamage(int dmg);        // 受伤：HP 不允许掉到 0 以下
    void spendAmmo(int n = 1);       // 消耗弹药：不允许扣成负数
    void heal(int n);                // 回血：HP 不允许超过上限（工程修理用）

    // ---- 兵种差异：纯虚函数 = 子类必须实现的"合同" ----
    virtual int maxRange()  const = 0;   // 攻击射程（曼哈顿距离内可打）
    virtual int moveRange() const = 0;   // 每回合最多移动几格
    virtual int damage()    const = 0;   // 单次攻击伤害
    virtual int maxAmmo() const { return ammo_; }   // 基类给默认值
    void refillAmmo(int n); 

    // ---- 特殊能力：默认"不会修理"，只有工程兵 override 成 true ----
    virtual bool canRepair()    const { return false; }
    virtual int  repairAmount() const { return 0; }

protected:
    
    Team   team_;
    string name_;
    int    hp_;       // 当前生命
    int    maxHp_;    // 生命上限
    int    ammo_;     // 当前弹药
};

// ============================================================
//  三种兵：只 override 有差异的地方，通用部分全部继承
//  （数值待定 —— 实现 Robot.cpp 时先用占位值，最后一起改）
// ============================================================
class Infantry : public Robot {
public:
    Infantry(Team team);             
    int maxRange()  const override;
    int moveRange() const override;
    int damage()    const override;
    int maxAmmo() const override;
};

class Sniper : public Robot {
public:
    Sniper(Team team);
    int maxRange()  const override;
    int moveRange() const override;
    int damage()    const override;
    int maxAmmo() const override;
};

class Engineer : public Robot {
public:
    Engineer(Team team);
    int maxRange()  const override;
    int moveRange() const override;
    int damage()    const override;
    bool canRepair()    const override;   
    int  repairAmount() const override;
    int maxAmmo() const override;
};

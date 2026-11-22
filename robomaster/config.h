#pragma once

namespace cfg {


constexpr int kBaseHp = 200;

//回合上限 

constexpr int kMaxHalfTurns = 60;

//补给区

constexpr int kSupplyHeal = 20;   
constexpr int kSupplyAmmo = 5;    

// 步兵
constexpr int kInfantryHp     = 100;
constexpr int kInfantryRange  = 2;
constexpr int kInfantryMove   = 2;
constexpr int kInfantryDamage = 20;
constexpr int kInfantryAmmo   = 20;

//大狙
constexpr int kSniperHp     = 80;
constexpr int kSniperRange  = 5;
constexpr int kSniperMove   = 1;
constexpr int kSniperDamage = 30;
constexpr int kSniperAmmo   = 10;

//工程
constexpr int kEngineerHp     = 100;
constexpr int kEngineerRange  = 1;
constexpr int kEngineerMove   = 2;
constexpr int kEngineerDamage = 5;
constexpr int kEngineerAmmo   = 20;
constexpr int kEngineerRepair = 10;

}   

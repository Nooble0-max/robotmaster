#include"Base.h"
using namespace std;
Base::Base(Team team,Pos pos,int maxHp):team_(team),pos_(pos),hp_(maxHp),maxHp_(maxHp){}
void Base::takeDamage(int dmg){
    hp_=hp_-dmg;
    if(hp_<0)
        hp_=0;
}

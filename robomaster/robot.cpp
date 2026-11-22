#include "Robot.h"
using namespace std;
Robot::Robot(Team team, const string& name, int maxHp):team_(team),name_(name),hp_(maxHp),maxHp_(maxHp),ammo_(0){}
void Robot::spendAmmo(int n){
    ammo_=ammo_-n;
    if(ammo_<0)
        ammo_=0;
}
void Robot::takeDamage(int dmg){
    hp_=hp_-dmg;
    if(hp_<0)
        hp_=0;
}
void Robot::heal(int hea){
    hp_=hp_+hea;
    if(hp_>maxHp_)
    hp_=maxHp_;
}
Infantry::Infantry(Team team):Robot(team,"步兵",100){ammo_=20;}
int Infantry::maxRange() const{
    return 1;
}
int Infantry::moveRange() const {
    return 2;
}
int Infantry::damage() const{
    return 20;
}
int Infantry::maxAmmo() const{
    return 20;
}

Sniper::Sniper(Team team):Robot(team,"大狙",80){ammo_=10;}
int Sniper::maxRange() const{
    return 1;
}
int Sniper::moveRange() const{
    return 1;
}
int Sniper::damage() const{
    return 30;
}
int Sniper::maxAmmo() const{
    return 10;
}

Engineer::Engineer(Team team ):Robot(team,"工程",100){ammo_=20;}
int Engineer::maxRange() const{
    return 1;
}
int Engineer::moveRange() const{
    return 2;
}
int Engineer::damage() const{
    return 5;
}
bool Engineer::canRepair()const{
    return true;
}
int Engineer::repairAmount()const{
    return 10;
}
int Engineer::maxAmmo() const{
    return 20;
}

void Robot::refillAmmo(int n){
    ammo_=ammo_+n;
    if(ammo_>maxAmmo())
        ammo_=maxAmmo();
}

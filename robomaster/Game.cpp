#include"Game.h"
#include "config.h"
using namespace std;

namespace {


bool canWalk(const Map& m, Pos from, Pos to, int steps) {
    if (from.r == to.r && from.c == to.c) {
        return true;
    }
    if (steps <= 0) {
        return false;
    }
    const int dr[4] = {1, -1, 0, 0};
    const int dc[4] = {0, 0, 1, -1};
    for (int i = 0; i < 4; ++i) {
        const Pos next{from.r + dr[i], from.c + dc[i]};
        if (next.r == to.r && next.c == to.c) {
            return true;          
        }
        if (!m.isEmpty(next)) {
            continue;             
        }
        if (canWalk(m, next, to, steps - 1)) {
            return true;
        }
    }
    return false;
}

}   

Game::Game()
    : redBase_(Team::Red, {0,3}, 200),      
      blueBase_(Team::Blue, {6,3}, 200),    
      currentTeam_(Team::Red),              
      turn_(0),
      logger_("game.log")
      
{
    map_.setTerrain({0,3}, Terrain::Base);   
    map_.setTerrain({6,3}, Terrain::Base);
    map_.setTerrain({3,1}, Terrain::Supply); 
    map_.setTerrain({3,5}, Terrain::Supply);

    // 新增（Step 8）：障碍。左右各一对 + 正中间一块，逼双方绕路。
    // 注意本项目没有视线判定，障碍挡路但不挡枪。
    map_.setTerrain({2,1}, Terrain::Obstacle);
    map_.setTerrain({2,5}, Terrain::Obstacle);
    map_.setTerrain({3,3}, Terrain::Obstacle);
    map_.setTerrain({4,1}, Terrain::Obstacle);
    map_.setTerrain({4,5}, Terrain::Obstacle);

    Robot* ri = new Infantry(Team::Red);  robots_.push_back(ri); map_.place(ri, {1,2});
    Robot* rs = new Sniper  (Team::Red);  robots_.push_back(rs); map_.place(rs, {1,4});
    Robot* re = new Engineer(Team::Red);  robots_.push_back(re); map_.place(re, {1,3});
    Robot* bi = new Infantry(Team::Blue); robots_.push_back(bi); map_.place(bi, {5,2});
    Robot* bs = new Sniper  (Team::Blue); robots_.push_back(bs); map_.place(bs, {5,4});
    Robot* be = new Engineer(Team::Blue); robots_.push_back(be); map_.place(be, {5,3});

    logger_.log("对局开始");
    logger_.log("红基地 HP " + to_string(redBase_.hp()) + "/" + to_string(redBase_.maxHp())
              + " @(" + to_string(redBase_.pos().r) + "," + to_string(redBase_.pos().c) + ")");
    logger_.log("蓝基地 HP " + to_string(blueBase_.hp()) + "/" + to_string(blueBase_.maxHp())
              + " @(" + to_string(blueBase_.pos().r) + "," + to_string(blueBase_.pos().c) + ")");
    for (Robot* r : robots_) {
        Pos p = map_.find(r);
        logger_.log(string(r->team() == Team::Red ? "红 " : "蓝 ") + r->name()
                  + " HP " + to_string(r->hp()) + "/" + to_string(r->maxHp())
                  + " 弹 " + to_string(r->ammo())
                  + " @(" + to_string(p.r) + "," + to_string(p.c) + ")");
    }
}

Game::~Game(){
    for(Robot*r:robots_)
        delete r;
}
void Game::endTurn(){
    if (currentTeam_==Team::Red)
        currentTeam_= Team::Blue;
    else
    currentTeam_=Team::Red;
    turn_++;
    logger_.log("回合over" );
}
bool Game::move(Robot*r ,Pos to){
     if (r->team() != currentTeam_) 
     return false;   
    if (!r->alive())               
    return false;   
    if (!map_.isEmpty(to))         
    return false;   
    if (manhattan(map_.find(r), to) > r->moveRange()) 
    return false; 
    if (!canWalk(map_, map_.find(r), to, r->moveRange()))
    return false;
    const Pos fromHere = map_.find(r);   
    map_.move(r, to);   
    logger_.log("回合" + std::to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 移动");
    logger_.log("    位置 (" + std::to_string(fromHere.r) + "," + std::to_string(fromHere.c) + ") -> ("+ std::to_string(to.r) + "," + std::to_string(to.c) + ")");
    return true;

}
bool Game::attack(Robot*r,Pos target){
    if(!r->alive())
    return false;
    if(!map_.inBounds(target))
    return false;
    if(currentTeam_!=r->team())
    return false;
    if(r->ammo()<=0)
    return false;
    if(manhattan(map_.find(r),target)>r->maxRange())
    return false;
    Robot*location=map_.at(target);
    if(location!=nullptr){
        if(location->team()==r->team())
        return false;
    const int hpBefore = location->hp();   // 新增：记下扣血前的 HP
    location->takeDamage(r->damage());
    r->spendAmmo(1);                 
    logger_.log("回合" + to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 攻击");
    logger_.log("    目标 " + std::string(location->team() == Team::Red ? "红 " : "蓝 ")
              + location->name() + " HP " + std::to_string(hpBefore) + "->"
              + std::to_string(location->hp()) + "，剩余弹药 " + std::to_string(r->ammo())
              + (location->alive() ? "" : "，已阵亡"));
    if(!location->alive())
        map_.remove(location);
    return true;
        
    }
    if(r->team() == Team::Red &&target.r == blueBase_.pos().r && target.c == blueBase_.pos().c){
        const int baseHpBefore = blueBase_.hp();   
        blueBase_.takeDamage(r->damage());
        r->spendAmmo(1);             
        logger_.log("回合" + to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 攻击");
        logger_.log("    目标 蓝基地 HP " + to_string(baseHpBefore) + "->"+ to_string(blueBase_.hp()) + "，剩余弹药 " + to_string(r->ammo()));
        return true;

    }
    if(r->team() == Team::Blue &&target.r == redBase_.pos().r && target.c == redBase_.pos().c){
        const int baseHpBefore2 = redBase_.hp();   
        redBase_.takeDamage(r->damage());
        r->spendAmmo(1);             
        logger_.log("回合" + to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 攻击");
        logger_.log("    目标 红基地 HP " + to_string(baseHpBefore2) + "->"+ to_string(redBase_.hp()) + "，剩余弹药 " + to_string(r->ammo()));
        return true;
    }
    return false;
}
bool Game::repair(Robot*r,Pos target){
    if(!r->alive())
    return false;
    if(r->team()!=currentTeam_)
    return false;
    if(!r->canRepair())
    return false;
    if(!map_.inBounds(target))
    return false;
    if(manhattan(map_.find(r),target)>r->maxRange())
    return false;
    Robot*location=map_.at(target);
    if(location==nullptr)
    return false;
    if(location->team()!=r->team())
    return false;
    if(location->hp()>=location->maxHp())
    return false;
    const int healBefore = location->hp();   
    location->heal(r->repairAmount());
    logger_.log("回合" + std::to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 修理");
    logger_.log("    目标 HP " + to_string(healBefore) + "->"
              + to_string(location->hp()));
    return true;
}
bool Game::over()const{
    
    if (turn_ >= cfg::kMaxHalfTurns)// 此行内容为AI新增（Step 8）：到回合上限也算结束（比基地 HP）
    return true;
    
    return !redBase_.alive()||!blueBase_.alive();
}
Team Game::winner()const{
    if (!redBase_.alive())  
    return Team::Blue;   
    if (!blueBase_.alive()) 
    return Team::Red;    
return Team::Red;


}


bool Game::supply(Robot*r){
    if(r==nullptr)
    return false;
    if(!r->alive())
    return false;
    if(r->team()!=currentTeam_)
    return false;
    Pos here = map_.find(r);
    if(here.r<0)                       
    return false;
    if(!map_.isSupply(here))
    return false;
    if(r->hp()>=r->maxHp() && r->ammo()>=r->maxAmmo())
    return false;
    r->heal(cfg::kSupplyHeal);
    r->refillAmmo(cfg::kSupplyAmmo);
    logger_.log("回合" + to_string(turn_) + " " +
            (r->team()==Team::Red ? "红" : "蓝") + " " +
            r->name() + " 补给");
    logger_.log("    HP +" + to_string(cfg::kSupplyHeal)
              + "，弹药 +" + to_string(cfg::kSupplyAmmo));
            return true;
}
// 新增：对局记录 —— 最终结果
void Game::logFinal(){
    logger_.log("======== 对局结束 ========");
    logger_.log("红基地 " + std::to_string(redBase_.hp()) + "/" + std::to_string(redBase_.maxHp())
              + "，蓝基地 " + std::to_string(blueBase_.hp()) + "/" + std::to_string(blueBase_.maxHp()));
    if(!redBase_.alive() || !blueBase_.alive()){
        logger_.log("结束原因：基地被摧毁");
    }else{
        logger_.log("结束原因：到达回合上限");
    }
    if(isDraw()){
        logger_.log("结果：平局");
        return;
    }
    Team win = (!redBase_.alive() || !blueBase_.alive()) ? winner() : winnerByHp();
    logger_.log(std::string("胜者：") + (win == Team::Red ? "红方" : "蓝方"));
}

bool Game::isDraw()const{
    if(!over())
    return false;
    if(!redBase_.alive() || !blueBase_.alive())
    return false;
    return redBase_.hp()==blueBase_.hp();
}


Team Game::winnerByHp()const{
    return blueBase_.hp() > redBase_.hp() ? Team::Blue : Team::Red;
}

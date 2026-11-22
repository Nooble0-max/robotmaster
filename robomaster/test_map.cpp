#include <iostream>
#include "Map.h"
#include "Robot.h"
using namespace std;

// 打印一个 Pos，方便看结果
void show(const char* label, Pos p) {
    cout << label << " = {" << p.r << "," << p.c << "}" << endl;
}

int main() {
    Map m;   // 空棋盘

    // 1. 边界判断
    cout << "inBounds({0,0})  = " << m.inBounds({0,0})  << " (期望1)" << endl;
    cout << "inBounds({6,6})  = " << m.inBounds({6,6})  << " (期望1)" << endl;
    cout << "inBounds({7,0})  = " << m.inBounds({7,0})  << " (期望0)" << endl;
    cout << "inBounds({0,-1}) = " << m.inBounds({0,-1}) << " (期望0)" << endl;

    // 2. 地形布置 + 查询
    m.setTerrain({2,3}, Terrain::Obstacle);
    m.setTerrain({4,4}, Terrain::Supply);
    m.setTerrain({5,5}, Terrain::Base);
    cout << "isObstacle({2,3}) = " << m.isObstacle({2,3}) << " (期望1)" << endl;
    cout << "isObstacle({2,2}) = " << m.isObstacle({2,2}) << " (期望0)" << endl;
    cout << "isSupply({4,4})   = " << m.isSupply({4,4})   << " (期望1)" << endl;
    cout << "isBase({5,5})     = " << m.isBase({5,5})     << " (期望1)" << endl;

    // 3. 占位 + find
    Robot* a = new Infantry(Team::Blue);
    Robot* b = new Engineer(Team::Red);
    m.place(a, {1,1});
    cout << "isOccupied({1,1}) = " << m.isOccupied({1,1}) << " (期望1)" << endl;
    cout << "isOccupied({2,2}) = " << m.isOccupied({2,2}) << " (期望0)" << endl;
    show("find(a)          ", m.find(a));   // 期望 {1,1}

    // 4. move
    m.move(a, {3,1});
    show("find(a) 移动后   ", m.find(a));   // 期望 {3,1}
    cout << "isOccupied({1,1}) = " << m.isOccupied({1,1}) << " (期望0)" << endl;

    // 5. isEmpty
    cout << "isEmpty({0,0}) = " << m.isEmpty({0,0}) << " (期望1, 空地)" << endl;
    cout << "isEmpty({2,3}) = " << m.isEmpty({2,3}) << " (期望0, 障碍)" << endl;
    cout << "isEmpty({4,4}) = " << m.isEmpty({4,4}) << " (期望1, 补给可进)" << endl;
    cout << "isEmpty({5,5}) = " << m.isEmpty({5,5}) << " (期望0, 基地)" << endl;
    cout << "isEmpty({3,1}) = " << m.isEmpty({3,1}) << " (期望0, 有a占)" << endl;
    cout << "isEmpty({9,9}) = " << m.isEmpty({9,9}) << " (期望0, 越界)" << endl;

    delete a;
    delete b;
    return 0;
}

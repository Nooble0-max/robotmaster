#include"Map.h"
Map::Map(){ for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            grid[i][j]   = Terrain::Empty;   
            robot[i][j] = nullptr;
        }
    }
void Map::setTerrain(Pos p,Terrain t){
    grid[p.r][p.c] = t;
}
bool Map::inBounds(Pos p)const{
    return p.r >= 0 && p.r < N && p.c >= 0 && p.c < N;
}
bool Map::isBase(Pos p)const{
    return grid[p.r][p.c] == Terrain::Base;  
}
bool Map::isEmpty(Pos p)const{
     return inBounds(p) && !isObstacle(p) && !isBase(p) && !isOccupied(p);
    }
bool Map::isObstacle(Pos p)const{
    return grid[p.r][p.c] == Terrain::Obstacle;
}
bool Map::isOccupied(Pos p)const{
    return robot[p.r][p.c] != nullptr; 
 }
bool Map::isSupply(Pos p)const{
    return grid[p.r][p.c] == Terrain::Supply;
 }
void Map::place(Robot*r ,Pos p){
    robot[p.r][p.c] = r;
}
Pos Map::find(const Robot*r)const{for(int i=0;i<N;i++)
                                    for(int j=0;j<N;j++)
                                    if (robot[i][j]==r)
                                        return {i,j};
                                    return {-1,-1};
}
void Map::move(Robot* r, Pos to) {
    Pos from = find(r);              
    robot[from.r][from.c] = nullptr; 
    robot[to.r][to.c] = r;           
}
Robot* Map::at(Pos p) const { 
    return robot[p.r][p.c];
 }
void Map::remove(Robot*r ){
    Pos p=find(r);
    robot[p.r][p.c]=nullptr;
}




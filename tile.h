#pragma once
#include "logic.h"

inline cube unityCube(1.0f);

class Tile{
public:
    vec3D position;
    vec3D angle;
    COLORREF color;
    BOOL faces[6] = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
    
    Tile(vec3D pos, COLORREF col, vec3D ang={0, 0, 0});
    static void CreateTiles(std::vector<Tile> &tilesPV, std::map<std::tuple<int, int, int>, COLORREF> &worldMap);
    static void PushTileSection(std::map<std::tuple<int, int, int>, COLORREF> &worldMap, vec3D initPos, vec3D finalPos, COLORREF color);
    operator std::string() const;
    friend void operator<<(std::ostream& os, Tile t);
    //~Tile();
};
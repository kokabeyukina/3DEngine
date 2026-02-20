#pragma once
#include "logic.h"

const inline cube UNITY_CUBE(1.0f);

class Tile{
public:
    vec3D position;
    vec3D angle;
    COLORREF color;
    BOOL faces[6];
    
    Tile(vec3D pos, COLORREF col, vec3D ang={0, 0, 0});
    //Adds to worldMap a solid chunck of tiles from initPos to finalPos
    static void PushTileSection(std::map<std::tuple<int, int, int>, COLORREF>& worldMap, vec3D initPos, vec3D finalPos, COLORREF color);
    //Deletes overlapping tile faces and passes it to tilesPV
    static void CreateTiles(std::vector<Tile>& tilesPV, std::map<std::tuple<int, int, int>, COLORREF>& worldMap);
    
    operator std::string() const;
};
#pragma once
#include "logic.h"

struct face{
    BOOL visible = TRUE;
    triangle plane[2];
};

class Tile{
public:
    vec3D position;
    vec3D angle;
    float size;
    COLORREF color;
    BOOL faces[6] = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
    
    Tile(vec3D pos, COLORREF col){
        position = pos;
        color = col;
        angle = {0, 0, 0};
    }
    friend std::ostream& operator<<(std::ostream& os, const Tile t);
    static void CreateTiles(std::vector<Tile> &tilesPV, std::vector<vec3D> cords, std::vector<COLORREF> colors){
        for(int i = 0; i < cords.size(); i++){
            Tile tile = Tile(cords[i], colors[i]);
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{1, 0, 0}) != cords.end()){
                tile.faces[0] = FALSE;
                //std::cout << "double face: " << cords[i] << " east\n";
            }
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{-1, 0, 0}) != cords.end()){
                tile.faces[1] = FALSE;
                //std::cout << "double face: " << cords[i] << " west\n";
            }
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{0, -1, 0}) != cords.end()){
                tile.faces[2] = FALSE;
                //std::cout << "double face: " << cords[i] << " up\n";
            }
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{0, 1, 0}) != cords.end()){
                tile.faces[3] = FALSE;
                //std::cout << "double face: " << cords[i] << " down \n";
            }
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{0, 0, 1}) != cords.end()){
                tile.faces[4] = FALSE;
                //std::cout << "double face: " << cords[i] << " north\n";
            }
            if(std::find(cords.begin(), cords.end(), cords[i]+vec3D{0, 0, -1}) != cords.end()){
                tile.faces[5] = FALSE;
                //std::cout << "double face: " << cords[i] << " south\n";
            }
            tilesPV.push_back(tile);
        }
    }
    static void PushTileSection(std::vector<vec3D> &cords, std::vector<COLORREF> &colors, vec3D initPos, vec3D finalPos, COLORREF color){
        for(float i = initPos.x; i <= finalPos.x; i++)
        for(float j = initPos.y; j <= finalPos.y; j++)
        for(float k = initPos.z; k <= finalPos.z; k++){
            vec3D vec = {i,j,k};
            if(std::find(cords.begin(), cords.end(), vec) == cords.end()){
                //std::cout << "granted: " << vec << std::endl;
                cords.push_back(vec);
                colors.push_back(color);
            }//else{std::cout << "denied:  " << vec << std::endl;}
        }
            
        

        //Tile::CreateTiles(tilesPV, cords, colors);
    }
    BOOL operator==(Tile t){
        return t.position == position;
    }
    BOOL operator==(vec3D v){
        return v == position;
    }
};
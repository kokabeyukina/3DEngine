#include "tile.h"

Tile::Tile(vec3D pos, COLORREF col, vec3D ang) : position(pos), angle(ang), color(col){}

void Tile::CreateTiles(std::vector<Tile> &tilesPV, std::map<std::tuple<int, int, int>, COLORREF> &worldMap){
    tilesPV.clear();
    tilesPV.reserve(worldMap.size());

    std::tuple<int, int, int> offsets[6] = {
        {1, 0, 0}, {-1, 0, 0}, 
        {0, -1, 0}, {0, 1, 0}, 
        {0, 0, 1}, {0, 0, -1}
    };

    for(auto& [pos, color] : worldMap){
        auto [x, y, z] = pos;

        Tile tile({
            (float)x * unityCube.a, 
            (float)y * unityCube.a, 
            (float)z * unityCube.a
        }, color);

        for(int i = 0; i < 6; i++){
            auto [ox, oy, oz] = offsets[i];

            if(worldMap.count({x + ox, y + oy, z + oz}))
                tile.faces[i] = FALSE;
        }

        tilesPV.push_back(tile);
    }
}

void Tile::PushTileSection(std::map<std::tuple<int, int, int>, COLORREF> &worldMap, vec3D initPos, vec3D finalPos, COLORREF color){
    for(int x = initPos.x; x <= finalPos.x; x++)
    for(int y = initPos.y; y <= finalPos.y; y++)
    for(int z = initPos.z; z <= finalPos.z; z++){

        // First block placed stays
        worldMap.insert({{x, y, z}, color});
        // for overwrite, use this instead:
        //worldMap[{x, y, z}] = color;
    }
}

Tile::operator std::string() const{
    return std::format("{{\n   position: {},\n   angle: {},\n   color: {{{}, {}, {}}}\n}}", 
        (std::string)position, (std::string)angle, (color & 0xff), ((color >> 8) & 0xff), ((color >> 16) & 0xff));
}

void operator<<(std::ostream& os, Tile t){
    os << (std::string)t;
};
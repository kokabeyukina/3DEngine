#include "window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow){
    Window window(hInstance, nCmdShow, ((std::string)lpCmdLine).find("-noGL") == std::string::npos);

    if(((std::string)lpCmdLine).find(".obj") != std::string::npos){
        FormatFileInput(lpCmdLine, [&window](std::string file, std::string mtl, vec3D pos){
            window.cam.meshes.push_back(ObjToMeshC(file, mtl, pos));
        });
    }else{
        std::map<std::tuple<int, int, int>, COLORREF> worldMap;

        //floor
        Tile::PushTileSection(worldMap, {-22, 4, -22}, {22, 4, 22}, RGB(255, 255, 255));

        //U
        Tile::PushTileSection(worldMap, {-20, -1, 20}, {-20, 3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-18, -1, 20}, {-18, 3, 20}, RGB(90, 90, 90));
        worldMap.insert({{-19,  3, 20}, RGB(90, 90, 90)});

        //S
        Tile::PushTileSection(worldMap, {-16, -1, 20}, {-14, -1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-16,  1, 20}, {-14,  1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-16,  3, 20}, {-14,  3, 20}, RGB(90, 90, 90));
        worldMap.insert({{-16, 0, 20}, RGB(90, 90, 90)});
        worldMap.insert({{-14, 2, 20}, RGB(90, 90, 90)});
        
        //E
        Tile::PushTileSection(worldMap, {-12, -1, 20}, {-12,  3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-11, -1, 20}, {-10, -1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-11,  3, 20}, {-10,  3, 20}, RGB(90, 90, 90));
        worldMap.insert({{-11, 1, 20}, RGB(90, 90, 90)});

        //.
        worldMap.insert({{-7, 3, 20}, RGB(90, 90, 90)});
        
        //O
        Tile::PushTileSection(worldMap, {-5, -1, 20}, {-5, 3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {-3, -1, 20}, {-3, 3, 20}, RGB(90, 90, 90));
        worldMap.insert({{-4, -1, 20}, RGB(90, 90, 90)});
        worldMap.insert({{-4,  3, 20}, RGB(90, 90, 90)});

        //B
        Tile::PushTileSection(worldMap, {-1, -1, 20}, {-1,  3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, { 0, -1, 20}, { 1, -1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, { 0,  1, 20}, { 2,  1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, { 0,  3, 20}, { 2,  3, 20}, RGB(90, 90, 90));
        worldMap.insert({{1, 0, 20}, RGB(90, 90, 90)});
        worldMap.insert({{2, 2, 20}, RGB(90, 90, 90)});

        //J
        Tile::PushTileSection(worldMap, {4,  2, 20}, {4, 3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {6, -1, 20}, {6, 3, 20}, RGB(90, 90, 90));
        worldMap.insert({{5,  3, 20}, RGB(90, 90, 90)});

        //F
        Tile::PushTileSection(worldMap, { 9, -1, 20}, { 9,  3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {10, -1, 20}, {11, -1, 20}, RGB(90, 90, 90));
        worldMap.insert({{10, 1, 20}, RGB(90, 90, 90)});
        
        //I
        Tile::PushTileSection(worldMap, {13, 1, 20}, {13, 3, 20}, RGB(90, 90, 90));
        worldMap.insert({{13, -1, 20}, RGB(90, 90, 90)});

        //L
        Tile::PushTileSection(worldMap, {15, -1, 20}, {15, 3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {16,  3, 20}, {17, 3, 20}, RGB(90, 90, 90));
        
        //E
        Tile::PushTileSection(worldMap, {19, -1, 20}, {19,  3, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {20, -1, 20}, {21, -1, 20}, RGB(90, 90, 90));
        Tile::PushTileSection(worldMap, {20,  3, 20}, {21,  3, 20}, RGB(90, 90, 90));
        worldMap.insert({{20, 1, 20}, RGB(90, 90, 90)});

        Tile::CreateTiles(window.cam.tiles, worldMap);
    }
    window.MainLoop();
    return window.msg.wParam;
}

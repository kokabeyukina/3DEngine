#include "logic.h"
#include "tile.h"

std::ostream& operator<<(std::ostream& os, const vec3D v){
    os << "{x: " << v.x << ", y: " << v.y << ", z: " << v.z << "}";
    return os;
};

COLORREF& operator*=(COLORREF& color, const vec3D v){
    int B = floor(color/65536); 
    int G = floor(color/256) - B*256; 
    int R = floor(color) - G*256 - B*65536;
    R *= v.x; G *= v.y; B *= v.z; 
    color = R+G*256+B*65536;
    return color;
};

std::ostream& operator<<(std::ostream& os, const triangle t){
    os << "{\n   0: " << t.p[0] << "\n   1: " << t.p[1] << "\n   2: " << t.p[2] << "\n}";
    return os;
};

std::ostream& operator<<(std::ostream& os, const Tile t){
    os << "{\n   position: {x: " << t.position.x << ", y: " << t.position.y << ", z: " << t.position.z << 
    "}\n   angle: {x:" << t.angle.x << ", y: " << t.angle.y << ", z: " << t.angle.z << "}\n   color: "<< std::hex << t.color << "\n}";
    return os;
};

std::ostream& operator<<(std::ostream& os, std::vector<float> v){
    os << "{" << v[1];
    for(int i = 1; i < v.size(); i++){
        os << ", " << v[i];
    }
    os << "}";
    return os;
};
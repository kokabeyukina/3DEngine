#pragma once
#define NOMINMAX
#include <math.h>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <chrono>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <charconv>
#include <string_view>
#include <cstring>
#include <set>
#include <format>

struct mat4x4{
    float m[4][4] = {0};

    mat4x4 inverse();
    //mat4x4 operator*(const mat4x4& mat) const;
};

struct vec3D{
    float x, y, z;

    //float len() const;

    vec3D normalize() const;
    vec3D rotate(const vec3D v) const;

    void operator+=(const vec3D v);
    void operator-=(const vec3D v);
    void operator*=(const vec3D v);
    void operator+=(const float n);
    void operator-=(const float n);

    BOOL operator==(const vec3D v) const;
    vec3D operator+(const vec3D v) const;
    vec3D operator-(const vec3D v) const;
    float operator*(const vec3D v) const; //Vector dot product
    vec3D operator%(const vec3D v) const; //Vector cross product
    vec3D operator+(const float n) const;
    vec3D operator-(const float n) const;
    vec3D operator*(const float n) const;
    vec3D operator/(const float n) const;
    vec3D operator*(const mat4x4& m) const;

    operator std::string() const;
    friend COLORREF operator*(const COLORREF color, const vec3D v);
};

struct triangle{
    vec3D p[3];
    vec3D n;
    COLORREF color;

    triangle();
    //The normal is calculated in the constructor
    triangle(const vec3D p0, const vec3D p1, const vec3D p2, COLORREF c=RGB(255, 255, 255));
    //Passes the given normal
    triangle(const vec3D p0, const vec3D p1, const vec3D p2, const vec3D normal, COLORREF c=RGB(255, 255, 255));
    triangle rotate(const vec3D ang) const;
    vec3D normal() const;

    void operator*=(const vec3D v);
    void operator+=(const float n);
    void operator-=(const float n);

    triangle operator+(const vec3D v) const;
    triangle operator-(const vec3D v) const;
    triangle operator+(const float n) const;
    triangle operator*(const mat4x4& m) const;
    
    operator std::string() const;
};

struct mesh{
    std::vector<triangle> tris;
};

//Creates a basic cube mesh used for the Tile class
struct cube{
    float a;
    float r;
    triangle m[12];
    
    cube(float edge=1.0f);
};

//Formats the command line 
void FormatFileInput(LPSTR lpCmdLine, std::function<void(std::string, std::string, vec3D)> func);
bool ParseVector(const std::string& input, vec3D& outPos);
//Converts obj file to mesh class
mesh ObjToMesh(std::string file, std::string mtl="", vec3D pos={0,0,0});
//Faster version of ObjToMesh
mesh ObjToMeshC(std::string file, std::string mtl="", vec3D pos={0,0,0});
//Loads materials from mtl file
std::map<std::string, COLORREF> LoadMaterials(std::string filename);

struct screenTri{ 
    int x, y; 
    float z; 
};

//Keeps the color and depth data of every pixel on the screen
struct PixelBuffer{
    uint32_t* pixels = nullptr;
    float* depth = nullptr;
    int w = 0, h = 0;
    BITMAPINFO bmi;

    void Resize(int width, int height);
    void Clear(COLORREF color);
    ~PixelBuffer();
};
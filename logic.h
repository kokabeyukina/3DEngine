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
    vec3D rotate(const vec3D& v) const;
    //vec3D rotate(const float x, const float y, const float z) const;
    BOOL operator==(const vec3D& v) const;
    void operator+=(const vec3D& v);
    void operator-=(const vec3D& v);
    void operator*=(const vec3D& v);
    void operator+=(const float n);
    void operator-=(const float n);
    //void operator/=(const float n);
    vec3D operator+(const vec3D& v) const;
    vec3D operator-(const vec3D& v) const;
    float operator*(const vec3D& v) const;
    vec3D operator%(const vec3D& v) const;
    vec3D operator+(const float n) const;
    vec3D operator-(const float n) const;
    vec3D operator*(const float n) const;
    vec3D operator/(const float n) const;
    vec3D operator*(const mat4x4& m) const;
    void operator*=(const mat4x4& m);
    operator std::string() const;
    friend void operator<<(std::ostream& os, const vec3D& v);
    friend void operator*=(COLORREF& color, const vec3D& v);
};

struct triangle{
    vec3D p[3];
    vec3D n;
    COLORREF color;

    triangle(){};
    triangle(const vec3D& p0, const vec3D& p1, const vec3D& p2, COLORREF c);
    triangle(const vec3D& p0, const vec3D& p1, const vec3D& p2, const vec3D& normal, COLORREF c);
    triangle rotate(const vec3D& v) const;
    vec3D normal() const;
    static vec3D normal(const vec3D& v0, const vec3D& v1, const vec3D& v2);
    void operator*=(const vec3D& v);
    void operator+=(const float n);
    void operator-=(const float n);
    triangle operator+(const float n) const;
    triangle operator+(const vec3D& v) const;
    triangle operator-(const vec3D& v) const;
    triangle operator*(const mat4x4& m) const;
    friend void operator<<(std::ostream& os, const triangle& t);
    operator std::string() const;
};

struct mesh{
    std::vector<triangle> tris;
};

struct cube{
    float a;
    float r;
    triangle m[12];
    
    cube(float edge=1.0f);
};

void FormatFileInput(int argc, wchar_t* args[], std::function<void(std::ifstream&)> func);
void FormatFileInput(LPSTR lpCmdLine, std::function<void(std::string, std::string, vec3D)> func);
bool ParseVector(const std::string& input, vec3D& outPos);
mesh ObjToMesh(std::string file, std::string mtl="", vec3D pos={0,0,0});
mesh ObjToMeshC(std::string file, std::string mtl="", vec3D pos={0,0,0});
std::map<std::string, COLORREF> LoadMaterials(std::string filename);

struct PixelBuffer{
    uint32_t* pixels = nullptr;
    float* depth = nullptr;
    int w = 0, h = 0;
    BITMAPINFO bmi;

    void Resize(int width, int height);
    void Clear(COLORREF color);
    ~PixelBuffer();
};

/*struct Formatter{
    static inline char buf[256];

    template<typename T>
    static std::string_view Format(std::string_view label, T value, int precision=2);
    static std::string_view Format(std::string_view label, const vec3D& v);
};*/
#pragma once
#include <math.h>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <windows.h>

struct mat4x4{
    float m[4][4] = {0};

    mat4x4 inverse(){
        mat4x4 matrix;
		matrix.m[0][0] = m[0][0]; matrix.m[0][1] = m[1][0]; matrix.m[0][2] = m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m[0][1]; matrix.m[1][1] = m[1][1]; matrix.m[1][2] = m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m[0][2]; matrix.m[2][1] = m[1][2]; matrix.m[2][2] = m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
    }
    mat4x4 operator*(mat4x4 &m2){
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m[r][0] * m2.m[0][c] + m[r][1] * m[1][c] + m[r][2] * m[2][c] + m[r][3] * m2.m[3][c];
		return matrix;
	}
};

struct vec3D{
    float x, y, z;

    float len(){
        return sqrtf(x*x + y*y + z*z);
    }
    
    vec3D normalize(){
        return *this/len();
    }

    vec3D rotate(vec3D v){
        mat4x4 matRotX, matRotY, matRotZ;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] =  cosf(v.x);
        matRotX.m[1][2] =  sinf(v.x);
        matRotX.m[2][1] = -sinf(v.x);
        matRotX.m[2][2] =  cosf(v.x);
        matRotX.m[3][3] = 1;

        matRotY.m[0][0] =  cosf(v.y);
        matRotY.m[0][2] = -sinf(v.y);
        matRotY.m[1][1] = 1;
        matRotY.m[2][0] =  sinf(v.y);
        matRotY.m[2][2] =  cosf(v.y);
        matRotY.m[3][3] = 1;

        matRotZ.m[0][0] =  cosf(v.z);
        matRotZ.m[0][1] =  sinf(v.z);
        matRotZ.m[1][0] = -sinf(v.z);
        matRotZ.m[1][1] =  cosf(v.z);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;
        return *this * matRotX * matRotY * matRotZ;
    }
    vec3D rotate(float x, float y, float z){
        mat4x4 matRotX, matRotY, matRotZ;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] =  cosf(x);
        matRotX.m[1][2] =  sinf(x);
        matRotX.m[2][1] = -sinf(x);
        matRotX.m[2][2] =  cosf(x);
        matRotX.m[3][3] = 1;

        matRotY.m[0][0] =  cosf(y);
        matRotY.m[0][2] = -sinf(y);
        matRotY.m[1][1] = 1;
        matRotY.m[2][0] =  sinf(y);
        matRotY.m[2][2] =  cosf(y);
        matRotY.m[3][3] = 1;

        matRotZ.m[0][0] =  cosf(z);
        matRotZ.m[0][1] =  sinf(z);
        matRotZ.m[1][0] = -sinf(z);
        matRotZ.m[1][1] =  cosf(z);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;
        return *this * matRotX * matRotY * matRotZ;
    }
    BOOL operator==(const vec3D v){
        BOOL rx = FALSE, ry = FALSE, rz = FALSE;
        if(x == v.x) rx = TRUE;
        if(y == v.y) ry = TRUE;
        if(z == v.z) rz = TRUE;
        return rx && ry && rz;
    }
    
    vec3D& operator-=(const vec3D& v){
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }
    vec3D& operator+=(const vec3D& v){
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }
    vec3D& operator/=(const float n){
        this->x /= n;
        this->y /= n;
        this->z /= n;
        return *this;
    }
    vec3D operator-(const vec3D v){
        vec3D r;
        r.x = x - v.x;
        r.y = y - v.y;
        r.z = z - v.z;
        return r;
    }
    vec3D operator+(const vec3D v){
        vec3D r;
        r.x = x + v.x;
        r.y = y + v.y;
        r.z = z + v.z;
        return r;
    }
    vec3D operator+(const float n){
        vec3D r;
        r.x = x + n;
        r.y = y + n;
        r.z = z + n;
        return r;
    }
    vec3D operator*(const float n){
        vec3D r;
        r.x = x * n;
        r.y = y * n;
        r.z = z * n;
        return r;
    }
    float operator*(const vec3D v){
        return x * v.x + y * v.y + z * v.z;
    }
    vec3D operator%(const vec3D v){
        vec3D r;
        r.x = y * v.z - z * v.y;
        r.y = z * v.x - x * v.z;
        r.z = x * v.y - y * v.x;
        return r;
    }
    vec3D operator/(const float n){
        vec3D r;
        r.x = x / n;
        r.y = y / n;
        r.z = z / n;
        return r;
    }
    vec3D operator*(const mat4x4 m){
        vec3D r;
        r.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + m.m[3][0];
        r.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + m.m[3][1];
        r.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + m.m[3][2];
        float w = this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + m.m[3][3];
        if(w != 0.0f) r /= w;
        return r;
    };
    friend std::ostream& operator<<(std::ostream& os, const vec3D v);
    friend COLORREF& operator*=(COLORREF& color, const vec3D v);
    friend std::ostream& operator<<(std::ostream& os, std::vector<float> v);
};

// std::ostream& operator<<(std::ostream& os, const vec3D v){
//     os << "{x: " << v.x << ", y: " << v.y << ", z: " << v.z << "}";
//     return os;
// };

// COLORREF& operator*=(COLORREF& color, const vec3D v){
//     int B = floor(color/65536); 
//     int G = floor(color/256) - B*256; 
//     int R = floor(color) - G*256 - B*65536;
//     R *= v.x; G *= v.y; B *= v.z; 
//     color = R+G*256+B*65536;
//     return color;
// };

struct triangle{
    vec3D p[3];
    COLORREF color;

    triangle rotate(vec3D v){
        mat4x4 matRotX, matRotY, matRotZ;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] =  cosf(v.x);
        matRotX.m[1][2] =  sinf(v.x);
        matRotX.m[2][1] = -sinf(v.x);
        matRotX.m[2][2] =  cosf(v.x);
        matRotX.m[3][3] = 1;

        matRotY.m[0][0] =  cosf(v.y);
        matRotY.m[0][2] = -sinf(v.y);
        matRotY.m[1][1] = 1;
        matRotY.m[2][0] =  sinf(v.y);
        matRotY.m[2][2] =  cosf(v.y);
        matRotY.m[3][3] = 1;

        matRotZ.m[0][0] =  cosf(v.z);
        matRotZ.m[0][1] =  sinf(v.z);
        matRotZ.m[1][0] = -sinf(v.z);
        matRotZ.m[1][1] =  cosf(v.z);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;
        return *this * matRotX * matRotY * matRotZ;
    }

    vec3D normal(){
        vec3D line1 = p[1] - p[0];
        vec3D line2 = p[2] - p[0];
        vec3D normal = line1%line2;
        return normal/normal.len();
    }

    triangle& operator-=(const float n){
        this->p[0].x -= n; this->p[0].y -= n; this->p[0].z -= n;
        this->p[1].x -= n; this->p[1].y -= n; this->p[1].z -= n;
        this->p[2].x -= n; this->p[2].y -= n; this->p[2].z -= n;
        return *this;
    }
    triangle& operator+=(const float n){
        this->p[0].x += n; this->p[0].y += n; this->p[0].z += n;
        this->p[1].x += n; this->p[1].y += n; this->p[1].z += n;
        this->p[2].x += n; this->p[2].y += n; this->p[2].z += n;
        return *this;
    }
    triangle& operator*=(const vec3D v){
        this->p[0].x *= v.x; this->p[0].y *= v.y; this->p[0].z *= v.z;
        this->p[1].x *= v.x; this->p[1].y *= v.y; this->p[1].z *= v.z;
        this->p[2].x *= v.x; this->p[2].y *= v.y; this->p[2].z *= v.z;
        return *this;
    }
    triangle operator+(const vec3D v){
        triangle r;
        r.p[0] = p[0] + v;
        r.p[1] = p[1] + v;
        r.p[2] = p[2] + v;
        return r;
    }
    triangle operator-(const vec3D v){
        triangle r;
        r.p[0] = p[0] - v;
        r.p[1] = p[1] - v;
        r.p[2] = p[2] - v;
        return r;
    }
    triangle operator*(const mat4x4 m){
        triangle r;
        r.p[0] = this->p[0] * m;
        r.p[1] = this->p[1] * m;
        r.p[2] = this->p[2] * m;
        return r;
    };
    friend std::ostream& operator<<(std::ostream& os, const triangle t);
};

// std::ostream& operator<<(std::ostream& os, const triangle t){
//     os << "{\n   0: " << t.p[0] << "\n   1: " << t.p[1] << "\n   2: " << t.p[2] << "\n}";
//     return os;
// };


struct mesh{
    std::vector<triangle> tris;
};
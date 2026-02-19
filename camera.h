#pragma once
#include "tile.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <thread>

class Camera{
private:
    /*
    * (float)screenHeight/(float)screenWidth*fov, 0,   0,                            0, 
    * 0,                                          fov, 0,                            0, 
    * 0,                                          0,   zFar / (zFar - zNear),        1, 
    * 0,                                          0,   -zNear*zFar / (zFar - zNear), 0
    */
    mat4x4 matProj;
    mat4x4 matView;


    float normalPace;
    float speedyPace;
    float sensiblity;
    float lightAng;

    std::vector<triangle> vecTrianglesToRaster;
    static mat4x4 MatPointAt(vec3D pos, vec3D target, vec3D up);
    void RasterizeTriangle(const triangle& tri, PixelBuffer& buf, int stripTop, int stripBottom);
    void Project(std::vector<triangle> &vecTrianglesToRaster, triangle& tri, vec3D pos, vec3D ang, COLORREF color);
    //void Project(std::vector<triangle> &vecTrianglesToRaster, mesh mesh, vec3D pos, vec3D ang);
    //void Project(std::vector<triangle> &vecTrianglesToRaster, Tile tile);
    inline static vec3D VectorIntersectPlane(vec3D &plane_p, vec3D &plane_n, vec3D &lineStart, vec3D &lineEnd);
    static int TriangleClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2);

public:
    HDC hdc;

    vec3D lookDir;
    vec3D position;
    vec3D lightDir;
    COLORREF BackGround;
    GLuint fontBase;

    int screenWidth; 
    int screenHeight;

    float fov;
    float zNear;

    std::vector<Tile> tiles;
    std::vector<mesh> meshes;
    
    Camera();
    void UpdateScreenSize(int w, int h);
    void UpdateViewAngle(float viewAngle);
    void UpdateZFarNear(float zFar, float zNear);
    void ChangeLightDir(vec3D direction);
    void DrawFrame(PixelBuffer& pb);
    void DrawFrameGL();
    void Write(int x, int y, std::string text, COLORREF color, HFONT font);
    void Write(int x, int y, std::string_view text, COLORREF color, HFONT font);
    void WriteGL(float x, float y, std::string text);
    void move(std::map<int, bool> keyStates, double elapsedTime);
    void lookAt(int x, int y);

    //~Camera();
};


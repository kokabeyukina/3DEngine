#pragma once
#include "tile.h"

class Camera{
public:
    mesh unityCube = {{
        // EAST
        { 0.5f, -0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f,  0.5f,  0.5f},
        { 0.5f, -0.5f, -0.5f,    0.5f,  0.5f,  0.5f,    0.5f, -0.5f,  0.5f},
        // WEST
        {-0.5f, -0.5f, 0.5f,    -0.5f,  0.5f,  0.5f,   -0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f,    -0.5f,  0.5f, -0.5f,   -0.5f, -0.5f, -0.5f},
        // TOP
        { 0.5f, -0.5f,  0.5f,   -0.5f, -0.5f,  0.5f,   -0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f,  0.5f,   -0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f},
        // BOTTOM
        {-0.5f,  0.5f, -0.5f,   -0.5f,  0.5f,  0.5f,    0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f,    0.5f,  0.5f,  0.5f,    0.5f,  0.5f, -0.5f},
        // NORTH
        { 0.5f, -0.5f,  0.5f,    0.5f,  0.5f,  0.5f,   -0.5f,  0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f,   -0.5f,  0.5f,  0.5f,   -0.5f, -0.5f,  0.5f},
        // SOUTH
        {-0.5f, -0.5f, -0.5f,   -0.5f,  0.5f, -0.5f,    0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f, -0.5f, -0.5f},
    }};
    mat4x4 matProj;
    mat4x4 matView;
    mat4x4 unityMat = {
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1
    };
    vec3D uCamera;
    float fTheta;
    HDC hdc;
    int screenWidth; 
    int screenHeight;
    float aspectRatio;
    float viewAngle;
    float fov;
    float zNear;
    float zFar;
    float qConst;
    vec3D lookDir;
    vec3D position;
    vec3D light_direction;
    std::vector<Tile> tiles;

    Camera(){
        viewAngle = 90.0f;
        fov = 1/tan((viewAngle * 0.5f) * (3.14159f / 180.0f));
        zNear = 0.1f;
        zFar = 10000.0f;
        qConst = zFar / (zFar - zNear);
        lookDir = {0, 0, 0};
        position = {0, -1, -3.0f};
        light_direction = {-0.2f, -0.2f, -1.0f};
        light_direction /= light_direction.len();
        fTheta = 0.0f;
        matProj = {
            aspectRatio*fov,   0,             0, 0, 
                          0, fov,             0, 0, 
                          0,   0,        qConst, 1, 
                          0,   0, -zNear*qConst, 1
        };
    }

    void updateScreenSize(int screenWidth, int screenHeight){
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->aspectRatio = (float)screenHeight / (float)screenWidth;
        matProj.m[0][0] = aspectRatio * fov;
    }

    void DrawLine(int x1, int y1, int x2, int y2){
        //HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        //SelectObject(hdc, hPen);
        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);
        //DeleteObject(hPen);
    }

    void DrawTriangle(COLORREF color, int x1, int y1, int x2, int y2, int x3, int y3, BOOL border=FALSE){
        if(color >= 0x1000000){
            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
            LineTo(hdc, x3, y3);
            LineTo(hdc, x1, y1);
        }else{
            HBRUSH hBrush = CreateSolidBrush(color);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            if(border){
                POINT points[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
                Polygon(hdc, points, 3);
            }else{
                HPEN hNullPen = (HPEN)GetStockObject(NULL_PEN);
                HPEN hOldPen = (HPEN)SelectObject(hdc, hNullPen);
                POINT points[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
                Polygon(hdc, points, 3);
                SelectObject(hdc, hOldPen);
                DeleteObject(hNullPen);
            }
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
        }
    }

    void write(int x, int y, std::string text, COLORREF color){
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
        std::vector<wchar_t> w_buffer(size_needed);
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &w_buffer[0], size_needed);
        SetTextColor(hdc, color);
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                VARIABLE_PITCH, L"Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        TextOutW(hdc, x, y, w_buffer.data(), w_buffer.size() - 1);
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }

    void DrawFrame(){
        fTheta += 0.0008f;
        std::vector<triangle> vecTrianglesToRaster;
        matView = MatPointAt(position, position+vec3D{0,0,1}.rotate(lookDir), {0, 1, 0}).inverse();
        for(Tile tile : tiles){
            Project(vecTrianglesToRaster, tile);
        }

        Project(vecTrianglesToRaster, unityCube, {3, -3, 0}, {fTheta, fTheta, fTheta});

        std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2){
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z)/3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z)/3.0f;
            return z1 > z2;
        });
        for(auto &triProjected : vecTrianglesToRaster){
            triangle clipped[2];
            std::list<triangle> listTriangles;
            listTriangles.push_back(triProjected);
            int nNewTriangles = 1;

            for(int p = 0; p < 4; p++){
                int nTrisToAdd = 0;
                while(nNewTriangles > 0){
                    triangle test = listTriangles.front();
                    listTriangles.pop_front();
                    nNewTriangles--;

                    switch(p){
                        case 0: nTrisToAdd = TriangleClipAgainstPlane({0, 0, 0}, {0, 1, 0}, test, clipped[0], clipped[1]); break;
                        case 1: nTrisToAdd = TriangleClipAgainstPlane({0, (float)screenHeight-1, 0}, {0, -1, 0}, test, clipped[0], clipped[1]); break;
                        case 2: nTrisToAdd = TriangleClipAgainstPlane({0, 0, 0}, {1, 0, 0}, test, clipped[0], clipped[1]); break;
                        case 3: nTrisToAdd = TriangleClipAgainstPlane({(float)screenWidth-1, 0, 0}, {-1, 0, 0}, test, clipped[0], clipped[1]); break;
                    }

                    for(int w = 0; w < nTrisToAdd; w++) listTriangles.push_back(clipped[w]);
                }
                nNewTriangles = listTriangles.size();
            }

            for(auto &t : listTriangles) DrawTriangle(t.color, t.p[0].x, t.p[0].y, 
                                                               t.p[1].x, t.p[1].y, 
                                                               t.p[2].x, t.p[2].y, 1);

            //DrawTriangle(triProjected.color, triProjected.p[0].x, triProjected.p[0].y, 
            //                                 triProjected.p[1].x, triProjected.p[1].y, 
            //                                 triProjected.p[2].x, triProjected.p[2].y, 1);
        }
        //for(auto &triProjected : vecTrianglesToRaster){
        //    std::stringstream ss; ss << (triProjected.p[0].z + triProjected.p[1].z + triProjected.p[2].z)/3.0f;
        //    write((triProjected.p[0].x+triProjected.p[1].x+triProjected.p[2].x)/3, (triProjected.p[0].y+triProjected.p[1].y+triProjected.p[2].y)/3-10, ss.str(), RGB(0,0,255));
        //}
    }
    mat4x4 rotate(mat4x4 m, vec3D v){
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
        return m * matRotX * matRotY * matRotZ;
    }

    void Project(std::vector<triangle> &vecTrianglesToRaster, mesh mesh, vec3D pos, vec3D ang){
        //mat4x4 matTrans;
		//matTrans.m[0][0] = 1.0f;
		//matTrans.m[1][1] = 1.0f;
		//matTrans.m[2][2] = 1.0f;
		//matTrans.m[3][3] = 1.0f;
		//matTrans.m[3][0] = pos.x;
		//matTrans.m[3][1] = pos.y;
		//matTrans.m[3][2] = pos.z;
        //mat4x4 matWorld = rotate(unityMat, ang)*matTrans;


        for(auto tri : mesh.tris){
            triangle triTranslated = tri.rotate(ang) + pos - position;
            //triangle triTranslated = tri*matWorld + pos - position;

            vec3D normal = triTranslated.normal();

            float penis = normal*(triTranslated.p[0]-position);

            std::stringstream ss; ss << normal*(triTranslated.p[0]-position);
            COLORREF wcor = RGB(255, 0, 0);
            if(penis < 0.0f){
                triangle triViewed = triTranslated * matView;
                float dp = normal*light_direction;
                if(dp < 0.0f) dp = 0.0f;

                int nClippedTriangles = 0;
                triangle clipped[2];
                nClippedTriangles = TriangleClipAgainstPlane({0, 0, -0.8f}, {0, 0, 1}, triViewed, clipped[0], clipped[1]);
                //std::cout << nClippedTriangles << std::endl;
                triangle triProjected;
                for (int n = 0; n < nClippedTriangles; n++){
                    triProjected = clipped[n] * matProj;
                    triProjected += 1.0f;
                    triProjected *= vec3D{(float)screenWidth, (float)screenHeight, 2}*0.5f;
                    
                    triProjected.color = 0x1000000;
                    if(triProjected.color < 0x1000000) triProjected.color *= vec3D{dp, dp, dp};
            //if((normal)*(triTranslated.p[0] - position) < 0.0f){
            //    wcor = RGB(255, 255, 255);
                    vecTrianglesToRaster.push_back(triProjected);
                }
                write((triProjected.p[0].x+triProjected.p[1].x+triProjected.p[2].x)/3, (triProjected.p[0].y+triProjected.p[1].y+triProjected.p[2].y)/3+50/(triProjected.p[0].z+triProjected.p[1].z+triProjected.p[2].z), ss.str(), wcor);
            }
        }
    }

    void Project(std::vector<triangle> &vecTrianglesToRaster, Tile tile){
        
        //for(auto tri : unityCube.tris){
        //    triangle triTranslated = tri.rotate(tile.angle) + tile.position - position;
        for(int i = 0; i < 6; i++){
        if(tile.faces[i]){
        for(int j = 0; j < 2; j++){
            triangle triTranslated = unityCube.tris[2*i+j].rotate(tile.angle) + tile.position - position;

            vec3D normal = triTranslated.normal();
            if(normal*(triTranslated.p[0]-position) < 0.0f){
                float dp = normal*light_direction;
                if(dp < 0.0f) dp = 0.0f;

                triangle triViewed = triTranslated * matView;
                int nClippedTriangles = 0;
                triangle clipped[2];
                nClippedTriangles = TriangleClipAgainstPlane({0, 0, -0.5f}, {0, 0, 1}, triViewed, clipped[0], clipped[1]);

                for (int n = 0; n < nClippedTriangles; n++){
                    triangle triProjected = clipped[n] * matProj;
                    triProjected += 1.0f;
                    triProjected *= vec3D{(float)screenWidth, (float)screenHeight, 2}*0.5f;
                    
                    triProjected.color = tile.color;
                    if(triProjected.color < 0x1000000) triProjected.color *= vec3D{dp, dp, dp};
                    vecTrianglesToRaster.push_back(triProjected);
                }
            }
        }}}
    }

    mat4x4 MatPointAt(vec3D pos, vec3D target, vec3D up){
        vec3D newForward = (target-pos).normalize();
        vec3D newUp = (up-newForward*(up*newForward)).normalize();
        vec3D newRight = newUp%newForward;
        return {
            newRight.x, newRight.y, newRight.z, 0,
            newUp.x, newUp.y, newUp.z, 0,
            newForward.x, newForward.y, newForward.z, 0,
            pos.x, pos.y, pos.z, 0,
        };
    }


    vec3D Vector_IntersectPlane(vec3D &plane_p, vec3D &plane_n, vec3D &lineStart, vec3D &lineEnd){
		plane_n = plane_n.normalize();
		float plane_d = -(plane_n*plane_p);
		float ad = lineStart*plane_n;
		float bd = lineEnd*plane_n;
		float t = (-plane_d - ad) / (bd - ad);
		vec3D lineStartToEnd = lineEnd-lineStart;
		vec3D lineToIntersect = lineStartToEnd*t;
		return lineStart+lineToIntersect;
	}

    int TriangleClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2){
		plane_n = plane_n.normalize();

		auto dist = [&](vec3D &p){
			vec3D n = p.normalize();
			return plane_n * p - plane_n*plane_p;
		};

		vec3D* inside_points[3];  int nInsidePointCount = 0;
		vec3D* outside_points[3]; int nOutsidePointCount = 0;

		float d0 = dist(in_tri.p[0]);
		float d1 = dist(in_tri.p[1]);
		float d2 = dist(in_tri.p[2]);

		if(d0 >= 0){ 
            inside_points[nInsidePointCount++] = &in_tri.p[0]; 
        }else{ 
            outside_points[nOutsidePointCount++] = &in_tri.p[0]; 
        }
		if(d1 >= 0){ 
            inside_points[nInsidePointCount++] = &in_tri.p[1]; 
        }else{ 
            outside_points[nOutsidePointCount++] = &in_tri.p[1]; 
        }
		if(d2 >= 0){ 
            inside_points[nInsidePointCount++] = &in_tri.p[2]; 
        }else{ 
            outside_points[nOutsidePointCount++] = &in_tri.p[2]; 
        }

		if(nInsidePointCount == 0){

			return 0;
		}

		if(nInsidePointCount == 3){
			out_tri1 = in_tri;

			return 1; 
		}

		if(nInsidePointCount == 1 && nOutsidePointCount == 2){
            out_tri1.color = in_tri.color;

			out_tri1.p[0] = *inside_points[0];

			out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1;
		}

		if(nInsidePointCount == 2 && nOutsidePointCount == 1){
			out_tri1.color = in_tri.color;
            out_tri2.color = in_tri.color;

			out_tri1.p[0] = *inside_points[0];
			out_tri1.p[1] = *inside_points[1];
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			out_tri2.p[0] = *inside_points[1];
			out_tri2.p[1] = out_tri1.p[2];
			out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2;
		}
        return -1;
	}
    //~Camera();
};


#include "camera.h"

Camera::Camera(){
    lookDir = {0, 0, 0};
    position = {0, 0, 0};
    BackGround = RGB(100, 100, 100);
    screenWidth = 0;
    screenHeight = 0;

    normalPace = 0.001f;
    speedyPace = 0.03f;
    sensiblity = 0.0025f;
    lightAng = 0.0f;

    ChangeLightDir({sin(lightAng), -0.5f, -cos(lightAng)});

    UpdateViewAngle(90.0f);
    matProj.m[2][3] = 1;
    UpdateZFarNear(10000.0f, 0.05f);
}

void Camera::UpdateScreenSize(int w, int h){
    screenWidth = w;
    screenHeight = h;
    matProj.m[0][0] = (float)screenHeight / (float)screenWidth * fov;
}

void Camera::UpdateViewAngle(float viewAngle){
    fov = 1/tan((viewAngle / 2) * (3.14159f / 180.0f));
    matProj.m[0][0] = (float)screenHeight / (float)screenWidth * fov;
    matProj.m[1][1] = fov;
}

void Camera::UpdateZFarNear(float zfar, float znear){
    zNear = znear;
    matProj.m[2][2] = zfar / (zfar - znear);
    matProj.m[3][2] = -znear*zfar / (zfar - znear);
}

void Camera::ChangeLightDir(vec3D direction){
    lightDir = direction;
    lightDir = lightDir.normalize();
}

void Camera::Write(int x, int y, std::string text, COLORREF color, HFONT font){
    std::wstring wtext(text.begin(), text.end()); 

    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    HFONT hOldFont = (HFONT)SelectObject(hdc, font);
    
    TextOutW(hdc, x, y, wtext.c_str(), (int)wtext.length());
    SelectObject(hdc, hOldFont);
}

mat4x4 Camera::MatPointAt(vec3D pos, vec3D target, vec3D up){
    vec3D newForward = (target - pos).normalize();
    vec3D newUp = (up - newForward * (up * newForward)).normalize();
    vec3D newRight = newUp % newForward;
    return {
        newRight.x, newRight.y, newRight.z, 0,
        newUp.x, newUp.y, newUp.z, 0,
        newForward.x, newForward.y, newForward.z, 0,
        pos.x, pos.y, pos.z, 0,
    };
}

void Camera::DrawFrame(PixelBuffer& pb){
    vecTrianglesToRaster.clear();

    matView = MatPointAt(position, position + vec3D{0, 0, 1}.rotate(lookDir), {0, 1, 0}).inverse();

    {
        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        std::vector<std::vector<triangle>> threadBuckets(numThreads);

        size_t chunk_size = tiles.size() / numThreads;

        for(size_t i = 0; i < numThreads; i++){
            size_t start = i * chunk_size;
            size_t end = (i == numThreads - 1) ? tiles.size() : (i + 1) * chunk_size;
            threads.emplace_back([&, i, start, end](){
                for(size_t j = start; j < end; j++)
                for(int k = 0; k < 6; k++)
                if(tiles[j].faces[k])
                for(int w = 0; w < 2; w++){
                    Project(threadBuckets[i], UNITY_CUBE.m[2*k+w], tiles[j].position, tiles[j].angle, tiles[j].color);
                }
            });
        }
        for(std::thread& t : threads) if(t.joinable()) t.join();

        for(std::vector<triangle>& bucket : threadBuckets)
            vecTrianglesToRaster.insert(vecTrianglesToRaster.end(), bucket.begin(), bucket.end());
    }
        
    for(mesh& m : meshes){
        size_t numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        std::vector<std::vector<triangle>> threadBuckets(numThreads);

        size_t chunk_size = m.tris.size() / numThreads;

        for(size_t i = 0; i < numThreads; i++){
            size_t start = i * chunk_size;
            size_t end = (i == numThreads - 1) ? m.tris.size() : (i + 1) * chunk_size;
            threads.emplace_back([&, i, start, end](){
                for(size_t j = start; j < end; j++){
                    //std::cout << (std::string)m.tris[j] << "mesh\n";
                    Project(threadBuckets[i], m.tris[j], {0, 0, 0}, {0, 0, 0}, m.tris[j].color);
                }
            });
        }
        for(std::thread& t : threads) if(t.joinable()) t.join();

        for(std::vector<triangle>& bucket : threadBuckets)
            vecTrianglesToRaster.insert(vecTrianglesToRaster.end(), bucket.begin(), bucket.end());
    }

    size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    int stripHeight = pb.h / numThreads;

    for(unsigned int i = 0; i < numThreads; i++){
        int startY = i * stripHeight;
        int endY = (i == numThreads - 1) ? pb.h : (i + 1) * stripHeight;

        // Launch Thread
        threads.emplace_back([&, startY, endY](){
            for(triangle& tri : vecTrianglesToRaster)
                RasterizeTriangle(tri, pb, startY, endY);
        });
    }
    
    for(std::thread& t : threads) if(t.joinable()) t.join();
}


void Camera::RasterizeTriangle(const triangle& tri, PixelBuffer& pb, int stripTop, int stripBottom){
    screenTri v[] = { 
        {(int)tri.p[0].x, (int)tri.p[0].y, tri.p[0].z},
        {(int)tri.p[1].x, (int)tri.p[1].y, tri.p[1].z},
        {(int)tri.p[2].x, (int)tri.p[2].y, tri.p[2].z} 
    };

    if(v[0].y > v[1].y) std::swap(v[0], v[1]);
    if(v[0].y > v[2].y) std::swap(v[0], v[2]);
    if(v[1].y > v[2].y) std::swap(v[1], v[2]);
    if(v[0].y >= stripBottom || v[2].y < stripTop) return;

    uint32_t color = ((tri.color & 0xFF) << 16) | (tri.color & 0xFF00) | ((tri.color & 0xFF0000) >> 16);

    for(int y = v[0].y; y <= v[2].y; y++){
        if(y < stripTop || y >= stripBottom) continue;
        float alpha = (float)(y - v[0].y) / (v[2].y - v[0].y + 1);
        int xa = v[0].x + (v[2].x - v[0].x) * alpha;
        float za = v[0].z + (v[2].z - v[0].z) * alpha;

        float beta, zb;
        int xb;
        if(y > v[1].y || v[1].y == v[0].y){
            beta = (float)(y - v[1].y) / (v[2].y - v[1].y + 1);
            xb = v[1].x + (v[2].x - v[1].x) * beta;
            zb = v[1].z + (v[2].z - v[1].z) * beta;
        }else{
            beta = (float)(y - v[0].y) / (v[1].y - v[0].y + 1);
            xb = v[0].x + (v[1].x - v[0].x) * beta;
            zb = v[0].z + (v[1].z - v[0].z) * beta;
        }
        
        if(y < 0 || y >= pb.h) continue;
        if(xa > xb){ 
            std::swap(xa, xb); 
            std::swap(za, zb); 
        }

        for(int x = std::max(0, xa); x <= std::min(pb.w - 1, xb); x++){
            float t = (xa == xb) ? 0 : (float)(x - xa) / (xb - xa);
            float z = za + t * (zb - za);
            int idx = y * pb.w + x;
            if(z < pb.depth[idx]){
                pb.depth[idx] = z;
                pb.pixels[idx] = color;
            }
        }
        //little faster aproach
        /*int startX = std::max(0, xa);
        if(xa == xb) continue;
        float zStep = (zb - za) / (float)(xb - xa);
        float currentZ = za + (startX - xa) * zStep;

        int rowOffset = y * pb.w;
        for (int x = startX; x <= std::min(pb.w - 1, xb); x++) {
            if (currentZ < pb.depth[rowOffset + x]) {
                pb.depth[rowOffset + x] = currentZ;
                pb.pixels[rowOffset + x] = color;
            }
            currentZ += zStep;
        }*/
    }
}

void Camera::DrawFrameGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0f, (float)screenWidth/screenHeight, zNear, 1000.0f);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    vec3D target = position + vec3D{0, 0, 1}.rotate(lookDir);
    gluLookAt(
        position.x, position.y, position.z,
        target.x,   target.y,   target.z,  
        0,          -1,         0
    );

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    float light_pos[] = {lightDir.x, lightDir.y, lightDir.z, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glBegin(GL_TRIANGLES); 

    for(Tile& tile : tiles)
    for(int i = 0; i < 6; i++)
    if(tile.faces[i])
    for(int j = 0; j < 2; j++){
        glNormal3f(UNITY_CUBE.m[2*i+j].n.x, UNITY_CUBE.m[2*i+j].n.y, UNITY_CUBE.m[2*i+j].n.z);

        glColor3f(
            GetRValue(tile.color) / 255.0f,
            GetGValue(tile.color) / 255.0f,
            GetBValue(tile.color) / 255.0f
        );

        glVertex3f(UNITY_CUBE.m[2*i+j].p[0].x + tile.position.x, UNITY_CUBE.m[2*i+j].p[0].y + tile.position.y, UNITY_CUBE.m[2*i+j].p[0].z + tile.position.z);
        glVertex3f(UNITY_CUBE.m[2*i+j].p[1].x + tile.position.x, UNITY_CUBE.m[2*i+j].p[1].y + tile.position.y, UNITY_CUBE.m[2*i+j].p[1].z + tile.position.z);
        glVertex3f(UNITY_CUBE.m[2*i+j].p[2].x + tile.position.x, UNITY_CUBE.m[2*i+j].p[2].y + tile.position.y, UNITY_CUBE.m[2*i+j].p[2].z + tile.position.z);
    }

    for(mesh& mesh : meshes)
    for(triangle& t : mesh.tris){
        glNormal3f(t.n.x, t.n.y, t.n.z);

        glColor3f(
            GetRValue(t.color) / 255.0f,
            GetGValue(t.color) / 255.0f,
            GetBValue(t.color) / 255.0f
        );

        glVertex3f(t.p[0].x, t.p[0].y, t.p[0].z);
        glVertex3f(t.p[1].x, t.p[1].y, t.p[1].z);
        glVertex3f(t.p[2].x, t.p[2].y, t.p[2].z);
    }

    glEnd(); 
    glDisable(GL_LIGHTING);
}

void Camera::BeginText(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glListBase(fontBase - 32);
}

void Camera::WriteGL(float x, float y, std::string text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, screenHeight - y);
    glCallLists((GLsizei)text.length(), GL_UNSIGNED_BYTE, text.c_str());
}

void Camera::EndText() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Camera::Project(std::vector<triangle>& vecTrianglesToRaster, const triangle& tri, vec3D pos, vec3D ang, COLORREF color){
    triangle triWorld = tri.rotate(ang) + pos; 
    triangle triViewed = triWorld * matView;

    if(triViewed.n * triViewed.p[0] < 0.0f){
        float dp = std::max(0.2f, cbrt(triWorld.normal() * lightDir));

        triangle clipped[2];
        int nClippedTriangles = TriangleClipAgainstPlane({0, 0, zNear}, {0, 0, 1}, triViewed, clipped[0], clipped[1]);
        
        triangle triProjected;
        for (int n = 0; n < nClippedTriangles; n++){
            triProjected = clipped[n] * matProj + 1.0f;
            triProjected *= vec3D{(float)screenWidth/2, (float)screenHeight/2, 1};
            triProjected.color = color == CLR_INVALID ? CLR_INVALID : color * vec3D{dp, dp, dp};
            
            vecTrianglesToRaster.push_back(triProjected);
        }
    }
}

int Camera::TriangleClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2){
	plane_n = plane_n.normalize();

	vec3D* inside_points[3];  
    int nInsidePointCount = 0;
	vec3D* outside_points[3]; 
    int nOutsidePointCount = 0;

    for(int i = 0; i < 3; i++){
        if(plane_n * in_tri.p[i] - plane_n * plane_p >= 0){ 
            inside_points[nInsidePointCount++] = &in_tri.p[i]; 
        }else{
            outside_points[nOutsidePointCount++] = &in_tri.p[i]; 
        }
    }

	if(nInsidePointCount == 0) return 0;

	if(nInsidePointCount == 3){
		out_tri1 = in_tri;
		return 1; 
	}

	if(nInsidePointCount == 1 && nOutsidePointCount == 2){
        out_tri1.color = in_tri.color;
        out_tri1.n = in_tri.n;

		out_tri1.p[0] = *inside_points[0];

		out_tri1.p[1] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1;
	}

	if(nInsidePointCount == 2 && nOutsidePointCount == 1){
        out_tri1.color = in_tri.color;
        out_tri2.color = in_tri.color;
		out_tri1.n = in_tri.n;
        out_tri2.n = in_tri.n;
        
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2;
	}
    return -1;
}

vec3D Camera::VectorIntersectPlane(vec3D plane_p, vec3D plane_n, vec3D lineStart, vec3D lineEnd){
	float ad = lineStart * plane_n;
	return lineStart + (lineEnd - lineStart) * (plane_n * plane_p - ad) / (lineEnd * plane_n - ad);
}

void Camera::Move(std::map<int, bool>& keyStates, double elapsedTime){
    float pace = normalPace;
    if(keyStates[VK_SHIFT]){
        pace = speedyPace;
    }
    if(keyStates['W']){
        position += vec3D{0, 0, 1}.rotate(lookDir)*pace*elapsedTime;
    }
    if(keyStates['S']){
        position -= vec3D{0, 0, 1}.rotate(lookDir)*pace*elapsedTime;
    }
    if(keyStates['D']){
        position += vec3D{1, 0, 0}.rotate(lookDir)*pace*elapsedTime;
    }
    if(keyStates['A']){
        position -= vec3D{1, 0, 0}.rotate(lookDir)*pace*elapsedTime;
    }
    if(keyStates[VK_CONTROL]){
        position.y += pace*elapsedTime;
    }
    if(keyStates[VK_SPACE]){
        position.y -= pace*elapsedTime;
    }
    if(keyStates['E']){
        lightAng += pace*elapsedTime;
        if(lightAng > 6.2831853071795864f) lightAng = 0.0f;
        ChangeLightDir({sin(lightAng), -0.5f, -cos(lightAng)});
    }
    if(keyStates['Q']){
        lightAng -= pace*elapsedTime;
        if(lightAng < -6.2831853071795864f) lightAng = 0.0f;
        ChangeLightDir({sin(lightAng), -0.5f, -cos(lightAng)});
    }
}

void Camera::LookAt(int x, int y){
    lookDir.x -= sensiblity * x;
    lookDir.y += sensiblity * y;

    if(lookDir.y > 6.2831853071795864f || lookDir.y < -6.2831853071795864f) lookDir.y = 0.0f;

    if(lookDir.x > 1.57f){
        lookDir.x = 1.57f;
    }else if(lookDir.x < -1.57f){
        lookDir.x = -1.57f;
    }
}

Camera::~Camera(){
    if(fontBase){
        glDeleteLists(fontBase, 96);
        fontBase = 0;
    }
}
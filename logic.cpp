#include "logic.h"

mat4x4 mat4x4::inverse(){
	return {
        m[0][0], m[1][0], m[2][0], 0.0f,
        m[0][1], m[1][1], m[2][1], 0.0f,
        m[0][2], m[1][2], m[2][2], 0.0f,
        -(m[3][0] * m[0][0] + m[3][1] * m[0][1] + m[3][2] * m[0][2]),
        -(m[3][0] * m[1][0] + m[3][1] * m[1][1] + m[3][2] * m[1][2]),
        -(m[3][0] * m[2][0] + m[3][1] * m[2][1] + m[3][2] * m[2][2]),
        1.0f
    };
}

/*mat4x4 mat4x4::operator*(const mat4x4& mat) const{
	mat4x4 matrix;
	for (int c = 0; c < 4; c++)
	for (int r = 0; r < 4; r++)
		matrix.m[r][c] = 
            m[r][0] * mat.m[0][c] + 
            m[r][1] * mat.m[1][c] + 
            m[r][2] * mat.m[2][c] + 
            m[r][3] * mat.m[3][c];
	return matrix;
}

float vec3D::len() const{
    return sqrtf(x*x + y*y + z*z);
}*/

vec3D vec3D::normalize() const{
    return *this/sqrtf(x*x + y*y + z*z);
}

vec3D vec3D::rotate(const vec3D v) const{
    mat4x4 matRotX = {0}, matRotY = {0}, matRotZ = {0};

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

void vec3D::operator+=(const vec3D v){
    x += v.x;
    y += v.y;
    z += v.z;
}

void vec3D::operator-=(const vec3D v){
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void vec3D::operator*=(const vec3D v){
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

void vec3D::operator+=(const float n){
    x += n;
    y += n;
    z += n;
}

void vec3D::operator-=(const float n){
    x -= n;
    y -= n;
    z -= n;
}


BOOL vec3D::operator==(const vec3D v) const{
    return x == v.x && y == v.y && z == v.z;
}

vec3D vec3D::operator+(const vec3D v) const{
    return {
        x + v.x,
        y + v.y,
        z + v.z
    };
}

vec3D vec3D::operator-(const vec3D v) const{
    return {
        x - v.x,
        y - v.y,
        z - v.z
    };
}

float vec3D::operator*(const vec3D v) const{
    return x * v.x + y * v.y + z * v.z;
}

vec3D vec3D::operator%(const vec3D v) const{
    return {
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    };
}

vec3D vec3D::operator+(const float n) const{
    return {
        x + n,
        y + n,
        z + n
    };
}

vec3D vec3D::operator-(const float n) const{
    return {
        x - n,
        y - n,
        z - n
    };
}

vec3D vec3D::operator*(const float n) const{
    return {
        x * n,
        y * n,
        z * n
    };
}

vec3D vec3D::operator/(const float n) const{
    return {
        x / n,
        y / n,
        z / n
    };
}

vec3D vec3D::operator*(const mat4x4& m) const{
    float w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + m.m[3][3];
    if(w == 0.0f) return {0, 0, 0};
    float invW = 1/w;
    return {
        (x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0])*invW,
        (x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1])*invW,
        (x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2])*invW
    };
}

vec3D::operator std::string() const{
    return std::format("{{x: {}, y: {}, z: {}}}", x, y, z);
}

COLORREF operator*(const COLORREF color, const vec3D v){
    int R = static_cast<int>((color & 0xFF) * v.z) & 0xFF;
    int G = static_cast<int>(((color >> 8) & 0xFF) * v.y) & 0xFF;
    int B = static_cast<int>(((color >> 16) & 0xFF) * v.x) & 0xFF;
    return (B << 16) | (G << 8) | R;
}

triangle::triangle(){};

triangle::triangle(const vec3D p0, const vec3D p1, const vec3D p2, COLORREF c)
 : p(p0, p1, p2), color(c){
    n = normal();
}

triangle::triangle(const vec3D p0, const vec3D p1, const vec3D p2, const vec3D normal, COLORREF c)
 : p(p0, p1, p2), n(normal), color(c){}

triangle triangle::rotate(const vec3D ang) const{
    mat4x4 matRotX, matRotY, matRotZ;

    matRotX.m[0][0] = 1;
    matRotX.m[1][1] =  cosf(ang.x);
    matRotX.m[1][2] =  sinf(ang.x);
    matRotX.m[2][1] = -sinf(ang.x);
    matRotX.m[2][2] =  cosf(ang.x);
    matRotX.m[3][3] = 1;

    matRotY.m[0][0] =  cosf(ang.y);
    matRotY.m[0][2] = -sinf(ang.y);
    matRotY.m[1][1] = 1;
    matRotY.m[2][0] =  sinf(ang.y);
    matRotY.m[2][2] =  cosf(ang.y);
    matRotY.m[3][3] = 1;

    matRotZ.m[0][0] =  cosf(ang.z);
    matRotZ.m[0][1] =  sinf(ang.z);
    matRotZ.m[1][0] = -sinf(ang.z);
    matRotZ.m[1][1] =  cosf(ang.z);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;
    return *this * matRotX * matRotY * matRotZ;
}

vec3D triangle::normal() const{
    return ((p[1] - p[0])%(p[2] - p[0])).normalize();
}

void triangle::operator*=(const vec3D v){
    p[0] *= v;
    p[1] *= v;
    p[2] *= v;
}

void triangle::operator+=(const float n){
    p[0] += n;
    p[1] += n;
    p[2] += n;
}

void triangle::operator-=(const float n){
    p[0] -= n;
    p[1] -= n;
    p[2] -= n;
}

triangle triangle::operator+(const vec3D v) const{
    return {
        p[0] + v,
        p[1] + v,
        p[2] + v,
        n, color
    };
}

triangle triangle::operator-(const vec3D v) const{
    return {
        p[0] - v,
        p[1] - v,
        p[2] - v,
        n, color
    };
}

triangle triangle::operator+(const float num) const{
    return {
        p[0] + num,
        p[1] + num,
        p[2] + num,
        n, color
    };
}

triangle triangle::operator*(const mat4x4& m) const{
    return {
        p[0] * m,
        p[1] * m,
        p[2] * m,
        color
    };
}

triangle::operator std::string() const{
    return std::format("{{\n   0: {},\n   1: {},\n   2: {}\n}}", (std::string)p[0], (std::string)p[1], (std::string)p[2]);
}

cube::cube(float edge) : a(edge), r(edge/2){
    int triangleIndex = 0;
    for(int i = 0; i < 6; i++){
        int axis = i / 2;
        float side = (i % 2 == 0) ? 1.0f : -1.0f;

        vec3D v[4];
        for(int j = 0; j < 4; j++){
            float d = ((j & 1) ? 1.0f : -1.0f) * r;
            float b = ((j & 2) ? 1.0f : -1.0f) * r;

            if(axis == 0) v[j] = {side * r, d, b}; 
            if(axis == 1) v[j] = {d, -side * r, b};
            if(axis == 2) v[j] = {d, b, side * r}; 
        }

        if(side > 0){
            m[triangleIndex++] = {v[0], v[1], v[3]};
            m[triangleIndex++] = {v[0], v[3], v[2]};
        }else{
            m[triangleIndex++] = {v[0], v[3], v[1]};
            m[triangleIndex++] = {v[0], v[2], v[3]};
        }
    }
    /*// EAST
    m[0] =  {{ r, -r, -r}, { r,  r, -r}, { r,  r,  r}};
    m[1] =  {{ r, -r, -r}, { r,  r,  r}, { r, -r,  r}};
    // WEST
    m[2] =  {{-r, -r,  r}, {-r,  r,  r}, {-r,  r, -r}};
    m[3] =  {{-r, -r,  r}, {-r,  r, -r}, {-r, -r, -r}};
    // TOP
    m[4] =  {{ r, -r,  r}, {-r, -r,  r}, {-r, -r, -r}};
    m[5] =  {{ r, -r,  r}, {-r, -r, -r}, { r, -r, -r}};
    //BOTTOM
    m[6] =  {{-r,  r, -r}, {-r,  r,  r}, { r,  r,  r}};
    m[7] =  {{-r,  r, -r}, { r,  r,  r}, { r,  r, -r}};
    // NORTH
    m[8] =  {{ r, -r,  r}, { r,  r,  r}, {-r,  r,  r}};
    m[9] =  {{ r, -r,  r}, {-r,  r,  r}, {-r, -r,  r}};
    // SOUTH
    m[10] = {{-r, -r, -r}, {-r,  r, -r}, { r,  r, -r}};
    m[11] = {{-r, -r, -r}, { r,  r, -r}, { r, -r, -r}};*/
}

void FormatFileInput(LPSTR lpCmdLine, std::function<void(std::string, std::string, vec3D)> func){
    std::stringstream ss(lpCmdLine);
    std::string param, currentObj, currentMtl;
    vec3D currentPos;
    std::map<std::string, std::tuple<std::string, vec3D>> objs;

    while(ss >> std::quoted(param, '"', (char)0)){
        if(param.find(".obj") != std::string::npos || param.find(".OBJ") != std::string::npos){
            currentObj = param;
            currentMtl = "";
            currentPos = {0,0,0};
            objs[currentObj] = {currentMtl, currentPos};

        }else if(param == "-mtl"){
            if(currentObj.empty()){
                std::cerr << "Error: Target obj file must be specified before -mtl flag.\n";
                return; 
            }

            ss >> param;

            if(param.find(".mtl") == std::string::npos || param.find(".MTL") != std::string::npos){
                std::cout << "Error: -mtl requires a valid mtl file after it.\n";
                return;
            }

            currentMtl = param;
            objs[currentObj] = {currentMtl, currentPos};

        }else if(param == "-p"){
            if(currentObj.empty()){
                std::cerr << "Error: Target obj file must be specified before -p flag.\n";
                return; 
            }

            if(!(ss >> param) || !ParseVector(param, currentPos)){
                std::cerr << "Error: -p requires format x,y,z (e.g., -2,0.2,3) after it.\n";
                return; 
            }

            objs[currentObj] = {currentMtl, currentPos};
        }
    }
    for(auto [obj, params] : objs){
        auto [mtl, pos] = params;
        func(obj, mtl, pos);
        std::cout << obj << ": " << mtl << ", " << (std::string)pos << "\n";
    }
}

bool ParseVector(const std::string& input, vec3D& outPos){
    float x, y, z;
    char c1, c2;
    std::stringstream ss(input);
    
    if(!(ss >> x >> c1 >> y >> c2 >> z) || c1 != ',' || c2 != ',')
        return false;
    
    outPos = {x, -y, z};
    return true;
}

mesh ObjToMesh(std::string obj, std::string mtl, vec3D pos){
    mesh rMesh;
    std::vector<vec3D> tempVerts;
    std::map<std::string, COLORREF> materials;
    COLORREF currentColor = RGB(255, 255, 255);
    std::string line, key;
    std::ifstream file(obj);

    if(!file.is_open()) return rMesh;
    if(mtl != "") materials = LoadMaterials(mtl);

    while(std::getline(file, line)){
        if(line.empty()) continue;
        std::stringstream ss(line);
        ss >> key;

        if(key == "mtllib"){
            if(mtl != "") continue;

            std::string mtlFilename;
            ss >> mtlFilename;

            if(obj.rfind("\\") != std::string::npos){
                materials = LoadMaterials(obj.substr(0, obj.rfind("\\"))+"\\"+mtlFilename);
            }else if(obj.rfind("/") != std::string::npos){
                materials = LoadMaterials(obj.substr(0, obj.rfind("/"))+"/"+mtlFilename);
            }else{
                materials = LoadMaterials(".\\"+mtlFilename);
            }
            
        }else if(key == "usemtl"){
            std::string mtlName;
            ss >> mtlName;
            if(materials.count(mtlName)) currentColor = materials[mtlName];

        }else if(key == "v"){
            vec3D v;
            ss >> v.x >> v.y >> v.z;
            v.y *= -1;
            v.z *= -1;
            tempVerts.push_back(v);

        }else if(key == "f"){
            std::string segment;
            std::vector<int> faceIndices;

            while(ss >> segment){
                size_t slashPos = segment.find('/');
                std::string indexStr = slashPos == std::string::npos ? segment : segment.substr(0, slashPos);
                
                faceIndices.push_back(std::stoi(indexStr) - 1);
            }

            for(size_t i = 1; i < faceIndices.size() - 1; i++){
                rMesh.tris.push_back({
                    tempVerts[faceIndices[0]]+pos,
                    tempVerts[faceIndices[i]]+pos,
                    tempVerts[faceIndices[i + 1]]+pos,
                    currentColor
                });
            }
        }
    }
    file.close();
    return rMesh;
}

mesh ObjToMeshC(std::string obj, std::string mtl, vec3D pos) {
    mesh rMesh;
    std::vector<vec3D> tempVerts;

    FILE* file = fopen(obj.c_str(), "r");
    if (!file) return rMesh;

    std::map<std::string, COLORREF> materials;
    if (mtl != "") materials = LoadMaterials(mtl);
    COLORREF currentColor = RGB(255, 255, 255);

    char lineHeader[128];
    while(fscanf(file, "%s", lineHeader) != EOF){
        if(strcmp(lineHeader, "mtllib") == 0){
            if(mtl != "") continue;

            char mtlFilename[128];
            fscanf(file, "%s\n", mtlFilename);

            if(obj.rfind("\\") != std::string::npos){
                materials = LoadMaterials(obj.substr(0, obj.rfind("\\"))+"\\"+mtlFilename);
            }else if(obj.rfind("/") != std::string::npos){
                materials = LoadMaterials(obj.substr(0, obj.rfind("/"))+"/"+mtlFilename);
            }else{
                materials = LoadMaterials("./"+(std::string)mtlFilename);
            }
            
        }else if(strcmp(lineHeader, "usemtl") == 0){
            char mtlName[128];
            fscanf(file, "%s\n", mtlName);
            if (materials.count(mtlName)) currentColor = materials[mtlName];

        }else if(strcmp(lineHeader, "v") == 0){
            vec3D v;
            fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
            v.y *= -1; v.z *= -1;
            tempVerts.push_back(v);

        }else if(strcmp(lineHeader, "f") == 0){
            char line[256];
            fgets(line, 256, file);
            
            std::vector<int> faceIndices;
            char* token = strtok(line, " ");
            while(token){
                faceIndices.push_back(atoi(token) - 1);
                token = strtok(NULL, " ");
            }

            for(size_t i = 1; i < faceIndices.size() - 1; ++i){
                rMesh.tris.push_back({
                    tempVerts[faceIndices[0]]+pos,
                    tempVerts[faceIndices[i]]+pos,
                    tempVerts[faceIndices[i + 1]]+pos,
                    currentColor
                });
            }
        }
    }
    fclose(file);
    return rMesh;
}

std::map<std::string, COLORREF> LoadMaterials(std::string filename){
    std::map<std::string, COLORREF> materials;
    std::ifstream file(filename);
    
    if(!file.is_open()) return materials;

    std::string line, key;
    std::string currentMtlName;

    while(std::getline(file, line)){
        std::stringstream ss(line);
        ss >> key;

        if(key == "newmtl"){
            ss >> currentMtlName;
        }else if(key == "Kd"){ // Diffuse color 0.0 ~ 1.0
            float r, g, b;
            ss >> r >> g >> b;
            materials[currentMtlName] = RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
        }
    }
    file.close();
    return materials;
}

void PixelBuffer::Resize(int width, int height){
    if(pixels) delete[] pixels;
    if(depth) delete[] depth;
    w = width; 
    h = height;
    pixels = new uint32_t[w * h];
    depth = new float[w * h];

    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
}

void PixelBuffer::Clear(COLORREF color){
    uint32_t c = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color & 0xFF0000) >> 16);
    std::fill_n(pixels, w * h, c);
    std::fill_n(depth, w * h, 1e9f);
}

PixelBuffer::~PixelBuffer(){ 
    delete[] pixels; 
    delete[] depth; 
}
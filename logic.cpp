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

std::vector<std::string> formatArgs(std::string argLine){
    std::vector<std::string> args = {""};
    int argc = 0;
    char stopChar = ' ';
    for(int i = 0; i < argLine.length(); i++){
        if(argLine[i] == '"'){
            if(stopChar == ' '){
                stopChar = '"';
            }else{
                stopChar = ' ';
            }
        }
        if(argLine[i] == stopChar && i != 0){
            argc++;
            args.push_back("");
        }else if(argLine[i] != '"'){
            args[argc].push_back(argLine[i]);
        }
    }
    return args;
}

void formatFileInput(std::string argLine, std::function<void(std::ifstream&)> func){
    std::vector<std::string> args = formatArgs(argLine);
    for(int i = 0; i < args.size(); i++){
        std::cout << args[i] << std::endl;
        // std::string fName;
        // if(args[i][0] != '"') fName = args[i]; 
        // else fName = args[i].substr(1, args[i].size()-2);
        // std::cout << "fName: " << fName << std::endl;
        // std::ifstream file(fName);
        std::ifstream file(args[i]);
        if(file.is_open()){
            func(file);
            //std::string data;
            //while(std::getline(file, data)){
            //    std::cout << data << std::endl;
            //}
            file.close();
        }else{
            std::cout << "Error: Given arg is not a file\n";
        }
    }
}
#include "window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    if(lpCmdLine != "") formatFileInput(lpCmdLine, [](std::ifstream& file){
        std::string data;
        while(std::getline(file, data)){
            std::cout << "line: " << data << std::endl;
        }
    });

    Window window(hInstance, nCmdShow, 0, 0); //Fullscreen if width and height are 0
    return window.msg.wParam;
}

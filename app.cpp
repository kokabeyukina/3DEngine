#include "window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    Window window(hInstance, nCmdShow, 0, 0); // Fullscreen if width and height are 0
    return window.msg.wParam;
}

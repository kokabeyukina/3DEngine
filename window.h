#pragma once
#include "camera.h"
#include "resource.h"


class Window{
private:
    HINSTANCE hInstance;
    const wchar_t* className;
    HWND hwnd;
    int width;
    int height;
    int screenStyle;
    std::map<int, bool> keyStates;
    bool isActive;

    double maxMsPerFrame;
    double deltaTime;
    double fps;

    HFONT hMainFont;

    PixelBuffer pb; 
    HGLRC hRC;
    HDC hdcGL;
    bool usingGL;

    void RegisterWindowClass();
    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT WndProcGL(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void TrapCursor(HWND hwnd);
    static void ReleaseCursor();

public:
    Camera cam;
    MSG msg;

    //Window constructor. Set w and h to 0 for fullscreen
    Window(HINSTANCE hInstance, int nCmdShow, bool GL=true, int w=0, int h=0);
    void MainLoop();
    void SetFPSLimit(float targetFPS=165.0f);
    ~Window();
};
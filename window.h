#pragma once
#include "camera.h"
#include "resource.h"


class Window{
public:
    int width;
    int height;
    HINSTANCE hInstance;
    const wchar_t *className;
    MSG msg;
    int screenStyle;
    Camera cam = Camera();
    COLORREF BackGround;
    double targetFPS;
    double msPerFrame;
    double fps;
    float pace = 0.01f;
    std::map<int, bool> keyStates;
    float MouseX = 0;
    float MouseY = 0;
    Window(HINSTANCE hInstance, int nCmdShow, int w, int h){
        className = L"WindowClass";
        this->hInstance = hInstance;
        BackGround = RGB(50,50,50);
        if(!w || !h){
            this->width = GetSystemMetrics(SM_CXSCREEN);
            this->height = GetSystemMetrics(SM_CYSCREEN);
            screenStyle = WS_POPUP | WS_VISIBLE; // Fullscreen
        }
        else{
            this->width = w;
            this->height = h;
            screenStyle = WS_OVERLAPPEDWINDOW;
        }
        cam.updateScreenSize(width, height);
        if(!RegisterWindowClass()) PostQuitMessage(0);
        HWND hwnd = CreateWindowExW(
            0,
            className, L"LifeNashi Beta",
            screenStyle,
            0, 0, width, height,
            NULL, NULL, hInstance, this
        );
        if(hwnd == NULL) PostQuitMessage(0);
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
        targetFPS = 165.0;
        msPerFrame = 1000.0 / targetFPS;

        std::vector<vec3D> cords;
        std::vector<COLORREF> colors;
        Tile::PushTileSection(cords, colors, {0, 0, 0}, {7, 0, 0}, RGB(200, 100, 100));
        Tile::PushTileSection(cords, colors, {0, -7, 0}, {0, -1, 0}, RGB(200, 100, 100));
        Tile::PushTileSection(cords, colors, {0, 0, 1}, {0, 0, 7}, RGB(200, 100, 100));
        Tile::PushTileSection(cords, colors, {10, -5, 5}, {10, 0, 10}, RGB(200, 100, 100));
        Tile::PushTileSection(cords, colors, {12, -5, 5}, {12, 0, 10}, RGB(200, 100, 100));
        Tile::PushTileSection(cords, colors, {14, -5, 5}, {14, 0, 10}, RGB(200, 100, 100));
        Tile::CreateTiles(cam.tiles, cords, colors);


        MainLoop(hwnd);
    }

    BOOL RegisterWindowClass(){
        WNDCLASSEXW wc;
        wc.cbSize        = sizeof(WNDCLASSEXW);
        wc.style         = 0;
        wc.lpfnWndProc   = WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 32, 32, 0);
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL; //(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = className;
        wc.hIconSm       = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
        if(!RegisterClassExW(&wc)){
            MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
        }
        return TRUE;
    }

    void MainLoop(HWND hwnd){
        auto lastFrameTime = std::chrono::high_resolution_clock::now();
        while(true){
            if(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)){
                if(msg.message == WM_QUIT) break;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }else{
                auto currentTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> deltaTime = currentTime - lastFrameTime;
                if(deltaTime.count() >= msPerFrame){
                    fps = 1000.0/deltaTime.count();
                    lastFrameTime = currentTime;
                    OnUpdate();
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
        }
    }

    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
        Window *pThis;
        switch(msg){
            case WM_NCCREATE:
                pThis = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
                return DefWindowProc(hwnd, msg, wParam, lParam);
            case WM_ERASEBKGND:
                return 1;
            case WM_CLOSE:
                DestroyWindow(hwnd);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            case WM_PAINT:{
                pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                RECT rect;
                GetClientRect(hwnd, &rect);
                int w = rect.right - rect.left;
                int h = rect.bottom - rect.top;
                HDC memDC = CreateCompatibleDC(hdc);
                pThis->cam.hdc = memDC;
                HBITMAP memBM = CreateCompatibleBitmap(hdc, w, h);
                HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);
                // Fill background in memory DC
                HBRUSH hBrush = CreateSolidBrush(pThis->BackGround);
                FillRect(memDC, &rect, hBrush);
                DeleteObject(hBrush);
                //char buffer[50];
                //sprintf(buffer, "FPS: %d", (int)fps);
                //SetTextColor(hdc, RGB(255, 255, 255)); // White text
                //SetBkMode(hdc, TRANSPARENT); // No background behind the text
                //TextOut(hdc, 10, 10, buffer, strlen(buffer));
                pThis->cam.DrawFrame();
                std::stringstream ss; ss << "FPS: " << pThis->fps;
                pThis->cam.write(10, 10, ss.str(), RGB(255, 255, 255));
                std::stringstream os; os << "position: " << pThis->cam.position;
                pThis->cam.write(10, 30, os.str(), RGB(255, 255, 255));
                std::stringstream as; as << "angle: " << pThis->cam.lookDir;
                pThis->cam.write(10, 50, as.str(), RGB(255, 255, 255));
                BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
                // Cleanup
                SelectObject(memDC, oldBM);
                DeleteObject(memBM);
                DeleteDC(memDC);
                EndPaint(hwnd, &ps);
                //InvalidateRect(hwnd, NULL, FALSE);
                break;
            }
            case WM_KEYDOWN:
                pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                if(!(lParam & (1 << 30))){ 
                    pThis->keyStates[wParam] = true;
                }
                if(wParam == VK_ESCAPE){
                    PostQuitMessage(0);
                }
                break;
            case WM_KEYUP:
                pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                pThis->keyStates[wParam] = false;
                break;
            case WM_LBUTTONDOWN:
                pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                pThis->MouseX = LOWORD(lParam);
                pThis->MouseY = HIWORD(lParam);
                break;
            case WM_MOUSEMOVE:
                if(wParam == MK_LBUTTON){
                    pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                    float xPos = LOWORD(lParam);
                    float yPos = HIWORD(lParam);
                    pThis->cam.lookDir.y += 0.004f*(xPos-pThis->MouseX);
                    pThis->cam.lookDir.x -= 0.004f*(yPos-pThis->MouseY);
                    pThis->MouseX = xPos;
                    pThis->MouseY = yPos;
                }
                break;
            default:
                //std::cout << "unknown msg: " << msg << "\n";
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
    void OnUpdate(){
        if(keyStates['W']){
            cam.position += vec3D{0, 0, 1}.rotate(cam.lookDir)*pace;
        }
        if(keyStates['S']){
            cam.position -= vec3D{0, 0, 1}.rotate(cam.lookDir)*pace;
        }
        if(keyStates['D']){
            cam.position += vec3D{1, 0, 0}.rotate(cam.lookDir)*pace;
        }
        if(keyStates['A']){
            cam.position -= vec3D{1, 0, 0}.rotate(cam.lookDir)*pace;
        }
        if(keyStates[VK_CONTROL]){
            cam.position.y += pace;
        }
        if(keyStates[VK_SPACE]){
            cam.position.y -= pace;
        }
    }
};
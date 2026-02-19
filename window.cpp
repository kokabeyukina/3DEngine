#include "window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, bool GL, int w, int h) : hInstance(hInstance), usingGL(GL){
    //this->hInstance = hInstance;
    //usingGL = GL;
    className = L"MainWindow";
    
    if(w && h){
        width = w;
        height = h;
        screenStyle = WS_OVERLAPPEDWINDOW;
    }else{
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        screenStyle = WS_POPUP | WS_VISIBLE; // Fullscreen
    }

    cam.position = {0, 0, -5};
    cam.UpdateScreenSize(width, height);
    cam.ChangeLightDir({sin(0.0f), -0.5f, -cos(0.0f)});

    RegisterWindowClass();
    hwnd = CreateWindowExW(
        0, className, L"3D Engine B1.0",
        screenStyle, 0, 0, width, height,
        NULL, NULL, hInstance, this
    );
    if(hwnd == NULL) PostQuitMessage(0);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    setFPSLimit();

    cam.BackGround = RGB(30, 30, 30);
    hMainFont = CreateFontW(
        23, 0, 0, 0, 
        FW_BOLD, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Consolas"
    );


    if(!usingGL) return;

    hdcGL = GetDC(hwnd);
    cam.hdc = hdcGL;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 
        24, // 24-bit Z-Buffer (Depth)
        8,  // Stencil Buffer
        0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    int pixelFormat = ChoosePixelFormat(hdcGL, &pfd);
    if(pixelFormat == 0){
        MessageBoxW(NULL, L"Could not find a suitable pixel format.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        PostQuitMessage(0);
    }
    SetPixelFormat(hdcGL, pixelFormat, &pfd);

    hRC = wglCreateContext(hdcGL);
    if(hRC == NULL){
        MessageBoxW(NULL, L"Could not create OpenGL rendering context.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        PostQuitMessage(0);
    }
    wglMakeCurrent(hdcGL, hRC);
    SelectObject(hdcGL, hMainFont);
    cam.fontBase = glGenLists(96);
    wglUseFontBitmaps(hdcGL, 32, 96, cam.fontBase);
}

void Window::RegisterWindowClass(){
    WNDCLASSEXW wc;
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = 0;
    wc.lpfnWndProc   = usingGL ? WndProcGL : WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 32, 32, 0);
    wc.hCursor       = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = className;
    wc.hIconSm       = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
    
    if(!RegisterClassExW(&wc)){
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        PostQuitMessage(0);
    }
}

void Window::MainLoop(){
    auto drawer = usingGL ? [](Window& w){
        w.cam.DrawFrameGL(); 
        w.cam.WriteGL(10, 30, "FPS: "+std::to_string(w.fps));
        SwapBuffers(w.cam.hdc);
    } : [](Window& w){
        InvalidateRect(w.hwnd, NULL, FALSE); 
    };
    if(usingGL){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glClearColor(
            GetRValue(cam.BackGround) / 255.0f, 
            GetGValue(cam.BackGround) / 255.0f, 
            GetBValue(cam.BackGround) / 255.0f, 
            1.0f
        );
    }
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while(true){
        if(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)){
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }else{
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> dT = (currentTime - lastFrameTime);
            deltaTime = dT.count();

            if(deltaTime >= maxMsPerFrame){
                fps = 1000.0/deltaTime;
                lastFrameTime = currentTime;
                OnUpdate();
                drawer(*this);
            }
        }
    }
}


LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    Window* thisPtr;
    switch(msg){
        case WM_NCCREATE:
            thisPtr = static_cast<Window*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisPtr));
            return DefWindowProcW(hwnd, msg, wParam, lParam);

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->isActive = LOWORD(wParam) != WA_INACTIVE;
            if(thisPtr->isActive){
                Window::TrapCursor(hwnd);
            }else{
                Window::ReleaseCursor();
            }
            break;

        case WM_PAINT:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            thisPtr->pb.Clear(thisPtr->cam.BackGround);
            thisPtr->cam.DrawFrame(thisPtr->pb);

            StretchDIBits(
                hdc, 0, 0, thisPtr->width, thisPtr->height, 
                0, 0, thisPtr->width, thisPtr->height, 
                thisPtr->pb.pixels, &thisPtr->pb.bmi, DIB_RGB_COLORS, SRCCOPY
            );

            thisPtr->cam.hdc = hdc;
            //thisPtr->cam.Write(10, 10, Formatter::Format("FPS: ", thisPtr->fps), RGB(255, 255, 255), thisPtr->hMainFont);
            //thisPtr->cam.Write(10, 35, Formatter::Format("Position: ", thisPtr->cam.position), RGB(255, 255, 255), thisPtr->hMainFont);
            //thisPtr->cam.Write(10, 60, Formatter::Format("Angle: ", thisPtr->cam.lookDir), RGB(255, 255, 255), thisPtr->hMainFont);
            //thisPtr->cam.Write(10, 85, Formatter::Format("Light: ", thisPtr->cam.lightDir), RGB(255, 255, 255), thisPtr->hMainFont);
            
            thisPtr->cam.Write(10, 10, "FPS: "+std::to_string(thisPtr->fps), RGB(255, 255, 255), thisPtr->hMainFont);
            thisPtr->cam.Write(10, 35, "Position: "+(std::string)thisPtr->cam.position, RGB(255, 255, 255), thisPtr->hMainFont);
            thisPtr->cam.Write(10, 60, "Angle: "+(std::string)thisPtr->cam.lookDir, RGB(255, 255, 255), thisPtr->hMainFont);
            thisPtr->cam.Write(10, 85, "Light: "+(std::string)thisPtr->cam.lightDir, RGB(255, 255, 255), thisPtr->hMainFont);
            
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_SIZE:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->width = LOWORD(lParam);
            thisPtr->height = HIWORD(lParam);

            thisPtr->pb.Resize(thisPtr->width, thisPtr->height);
            thisPtr->cam.UpdateScreenSize(thisPtr->width, thisPtr->height);
            break;
        }

        case WM_KEYDOWN:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if(!(lParam & (1 << 30))){ 
                thisPtr->keyStates[wParam] = true;
            }
            if(wParam == VK_ESCAPE) PostQuitMessage(0);
            break;
        }

        case WM_KEYUP:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->keyStates[wParam] = false;
            break;
        }

        case WM_MOUSEMOVE:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if(!thisPtr->isActive) break;

            RECT rect;
            GetClientRect(hwnd, &rect);
            int centerX = (rect.right - rect.left) / 2;
            int centerY = (rect.bottom - rect.top) / 2;

            int xPos = LOWORD(lParam);
            int yPos = HIWORD(lParam);

            if(xPos == centerX && yPos == centerY) break;

            thisPtr->cam.lookAt(yPos - centerY, xPos - centerX);

            POINT pt = {centerX, centerY};
            ClientToScreen(hwnd, &pt);
            SetCursorPos(pt.x, pt.y);
            break;
        }

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT Window::WndProcGL(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    Window* thisPtr;
    switch(msg){
        case WM_NCCREATE:
            thisPtr = static_cast<Window*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisPtr));
            return DefWindowProcW(hwnd, msg, wParam, lParam);

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->isActive = LOWORD(wParam) != WA_INACTIVE;
            if(thisPtr->isActive){
                Window::TrapCursor(hwnd);
            }else{
                Window::ReleaseCursor();
            }
            break;

        case WM_PAINT:{
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_SIZE:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->width = LOWORD(lParam);
            thisPtr->height = HIWORD(lParam);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0f, (float)thisPtr->cam.screenWidth/thisPtr->cam.screenHeight, thisPtr->cam.zNear, 1000.0f);
            glMatrixMode(GL_MODELVIEW);
            glViewport(0, 0, thisPtr->width, thisPtr->height);

            thisPtr->cam.UpdateScreenSize(thisPtr->width, thisPtr->height);
            break;
        }

        case WM_KEYDOWN:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if(!(lParam & (1 << 30))){ 
                thisPtr->keyStates[wParam] = true;
            }
            if(wParam == VK_ESCAPE) PostQuitMessage(0);
            break;
        }

        case WM_KEYUP:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            thisPtr->keyStates[wParam] = false;
            break;
        }

        case WM_MOUSEMOVE:{
            thisPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if(!thisPtr->isActive) break;

            RECT rect;
            GetClientRect(hwnd, &rect);
            int centerX = (rect.right - rect.left) / 2;
            int centerY = (rect.bottom - rect.top) / 2;

            int xPos = LOWORD(lParam);
            int yPos = HIWORD(lParam);

            if(xPos == centerX && yPos == centerY) break;

            thisPtr->cam.lookAt(yPos - centerY, xPos - centerX);

            POINT pt = {centerX, centerY};
            ClientToScreen(hwnd, &pt);
            SetCursorPos(pt.x, pt.y);
            break;
        }

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void Window::OnUpdate(){
    cam.move(keyStates, deltaTime);
}

void Window::TrapCursor(HWND hwnd){
    ShowCursor(FALSE);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    SetCursorPos((rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);
    ClipCursor(&rect);
}

void Window::ReleaseCursor(){
    ShowCursor(TRUE);
    ClipCursor(NULL);
}

void Window::setFPSLimit(float targetFPS){
    maxMsPerFrame = 1000.0 / targetFPS;
}

Window::~Window(){
    ReleaseCursor();

    if(hMainFont){
        DeleteObject(hMainFont);
        hMainFont = NULL;
    }

    if(hwnd){
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    UnregisterClassW(className, hInstance);
}
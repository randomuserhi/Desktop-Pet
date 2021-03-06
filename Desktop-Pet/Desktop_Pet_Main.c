#include "Deep.h"
#include "Deep_Containers.h"
#include "Deep_Math.h"

// https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
#define _WIN32_WINNT_WIN10 0x0A00
#include <Windows.h>

#include <gl\GL.h>
#include <gl\GLU.h>

#include <dwmapi.h>

void DrawCircle(int w, int h, double ori_x, double ori_y)
{
    glColor3f(0, 1, 1);
    glBegin(GL_POLYGON);                        // Middle circle
    double radius = 200;
    int resolution = 100;
    for (int i = 0; i <= resolution; i++) {
        double angle = 2 * 3.141592654 * i / resolution;
        double x = cos(angle) * radius;
        double y = sin(angle) * radius;
        glVertex2d(ori_x + x, ori_y + y);
    }
    glEnd();
}

HGLRC m_hrc;

BOOL initSC() {
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);

    return 0;
}

void resizeSC(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

BOOL renderSC(GLdouble w, GLdouble h, double x, double y) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();                           // Reset The Projection Matrix
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glOrtho(0, w, 0, h, -1, 1);                      // Set Up An Ortho Screen

    glTranslated(0, 0, 0);
    glScaled(1, 1, 0); // Scales from bottom left *bruh*

    DrawCircle(w, h, x, y);

    glPopMatrix();
    glFlush();

    return 0;
}

BOOL CreateHGLRC(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,                                // Version Number
      PFD_DRAW_TO_WINDOW |         // Format Must Support Window
      PFD_SUPPORT_OPENGL |         // Format Must Support OpenGL
      PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
      PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
      PFD_TYPE_RGBA,                    // Request An RGBA Format
      32,                               // Select Our Color Depth
      0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
      8,                                // An Alpha Buffer
      0,                                // Shift Bit Ignored
      0,                                // No Accumulation Buffer
      0, 0, 0, 0,                       // Accumulation Bits Ignored
      24,                               // 16Bit Z-Buffer (Depth Buffer)
      8,                                // Some Stencil Buffer
      0,                                // No Auxiliary Buffer
      PFD_MAIN_PLANE,                   // Main Drawing Layer
      0,                                // Reserved
      0, 0, 0                           // Layer Masks Ignored
    };

    HDC hdc = GetDC(hWnd);
    int PixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (PixelFormat == 0) {
        return FALSE;
    }

    BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
    if (bResult == FALSE) {
        return FALSE;
    }

    m_hrc = wglCreateContext(hdc);
    if (!m_hrc) {
        return FALSE;
    }

    ReleaseDC(hWnd, hdc);

    return TRUE;
}


int running = 1;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CHAR:
        if (wparam == VK_ESCAPE)
        {
            running = 0;
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_DESTROY:
        if (m_hrc) 
        {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(m_hrc);
        }
        running = 0;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, message, wparam, lparam);
    }
}

int main()
{
#ifdef DEEP_DEBUG_MEMORY
	Deep_Debug_Memory_Start();
#endif

	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_MINIMIZE);
	//ShowWindow(hWnd, SW_HIDE);

    const char* windowClassName = "Window in Console";
    WNDCLASS windowClass = { 0 };
    windowClass.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = NULL;
    windowClass.lpfnWndProc = WndProc;
    windowClass.lpszClassName = windowClassName;
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&windowClass))
        MessageBox(NULL, "Could not register class", "Error", MB_OK);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HWND windowHandle = CreateWindowEx(
        WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        windowClassName,
        NULL,
        WS_POPUP, //borderless
        0, //x coordinate of window start point
        0, //y start point
        w, //width of window; this function
        h, //height of the window
        NULL, //handles and such, not needed
        NULL,
        NULL,
        NULL);
    ShowWindow(windowHandle, SW_MAXIMIZE);
    SetLayeredWindowAttributes(windowHandle, 0, 255, LWA_ALPHA);

    DWM_BLURBEHIND bb = { 0 };
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(windowHandle, &bb);

    SetLayeredWindowAttributes(windowHandle, 0, 255, LWA_ALPHA);

    CreateHGLRC(windowHandle);

    HDC hdc = GetDC(windowHandle);
    wglMakeCurrent(hdc, m_hrc);
    initSC();
    resizeSC(1920, 1080);

    MSG messages;
    /*while (GetMessage(&messages, NULL, 0, 0) > 0)
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }*/

    double x = w/2.0f, y = h/2.0f;
    
    while (running)
    {
        if (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        else {
            //x += 0.1f;
            renderSC(w, h, x, y);
            SwapBuffers(hdc);
        }
    }

    ReleaseDC(windowHandle, hdc);
    DeleteObject(windowHandle); //doing it just in case

#ifdef DEEP_DEBUG_MEMORY
    Deep_Debug_Memory_End();
#endif

    return messages.wParam;
}


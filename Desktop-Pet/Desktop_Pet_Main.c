#include "Deep.h"
#include "Deep_Containers.h"
#include "Deep_Math.h"
#include "Deep_Engine.h"
#include "Deep_String.h"

// https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
#define _WIN32_WINNT_WIN10 0x0A00
#include <Windows.h>

#include <d3d12.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CHAR:
        if (wparam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_DESTROY:
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
    windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = NULL;
    windowClass.lpfnWndProc = WndProc;
    windowClass.lpszClassName = windowClassName;
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&windowClass))
        MessageBox(NULL, "Could not register class", "Error", MB_OK);

    HWND windowHandle = CreateWindowEx(
        WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        windowClassName,
        NULL,
        WS_POPUP, //borderless
        0, //x coordinate of window start point
        0, //y start point
        GetSystemMetrics(SM_CXSCREEN), //width of window; this function
        GetSystemMetrics(SM_CYSCREEN), //height of the window
        NULL, //handles and such, not needed
        NULL,
        NULL,
        NULL);
    ShowWindow(windowHandle, SW_MAXIMIZE);

    SetLayeredWindowAttributes(windowHandle, 0, 100, LWA_ALPHA);

    MSG messages;
    while (GetMessage(&messages, NULL, 0, 0) > 0)
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    DeleteObject(windowHandle); //doing it just in case

#ifdef DEEP_DEBUG_MEMORY
    Deep_Debug_Memory_End();
#endif

    return messages.wParam;
}


#include "KNR_ShaderInterop.h"
#include "buffer.h"
#include "command_buffer.h"
#include "graphics_context.h"
#include "render_commands.h"
#include "shader.h"
#include "texture.h"
#include "pipeline.h"
#include "graphics_application.h"
#include <window.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    KNR::RendererAPI::API graphicsAPI = KNR::RendererAPI::GetAPI();
    GraphicsApplication graphicsApp = GraphicsApplication();

    const wchar_t CLASS_NAME[] = L"KNT SANDBOX";
  
    //Set up window class
    WNDCLASS wnd;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hCursor = LoadCursor(0, IDC_ARROW);
    wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
    wnd.lpszMenuName = 0;
    wnd.style = 0;
    wnd.hbrBackground = 0;
    wnd.lpfnWndProc = WindowProc;
    wnd.hInstance = hInstance;
    wnd.lpszClassName = CLASS_NAME;

    RegisterClass(&wnd);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        CLASS_NAME,                     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    KNR::WindowDesc windowDesc = {};
    windowDesc.width = 1280;
    windowDesc.height = 720;
    windowDesc.hwnd = hwnd;
    windowDesc.instance = hInstance;

    graphicsApp.Initialise(windowDesc);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        //Bit of a hack but it'll do for now 
        graphicsApp.Update();
        graphicsApp.Render();
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
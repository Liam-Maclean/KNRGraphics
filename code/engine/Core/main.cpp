#include "buffer.h"
#include "command_buffer.h"
#include "graphics_context.h"
#include "render_commands.h"
#include "shader.h"
#include "texture.h"
#include <window.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    KNR::RendererAPI::API graphicsAPI = KNR::RendererAPI::GetAPI();

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
        L"Learn to Program Windows",    // Window text
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

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

int main()
{
    KNR::Buffer* buffer;
    KNR::Texture* texture;
    KNR::CommandBuffer* commandBuffer;

    KNR::WindowDesc windowDesc = {};
    windowDesc.width = 1920;
    windowDesc.height = 1080;

    //Initialize KNR (make sure this is done before initializing any KNR types)
    KNR::RenderCommand::Initialize(windowDesc);

    //Create a buffer using a descriptor
    KNR::BufferDescriptor bufferDesc = {};
    bufferDesc.bufferType = KNR::BufferUsageType::Vertex;
    bufferDesc.bufferAccessType = KNR::BufferAccessType::Default;
    bufferDesc.vertexBuffer.vertexDeclaration = 0;
    bufferDesc.size = (1024);
    buffer = KNR::Buffer::Create(bufferDesc);

    //Create a command buffer
    KNR::CommandBufferType commandBufferType = KNR::CommandBufferType::Graphics;
    commandBuffer = KNR::CommandBuffer::Create(commandBufferType);

    KNR::TextureDescriptor outputTargetDesc = {};
    outputTargetDesc.textureType = KNR::TextureType::Framebuffer;
    outputTargetDesc.textureUsage = KNR::TextureUsage::RenderTarget;
    outputTargetDesc.textureFormat = KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_FLOAT;
    outputTargetDesc.width = 1920;
    outputTargetDesc.height = 1080;
    outputTargetDesc.mipCount = 1;
    outputTargetDesc.samples = 1;
   

    KNR::Texture2D* outputTarget = KNR::Texture2D::Create(outputTargetDesc);

    while (true)
    {
        KNR::RenderCommand::BeginRender();
        KNR::RenderCommand::SetViewport(commandBuffer, 0, 0, 1920, 1080);

        KNR::RenderCommand::

        KNR::RenderCommand::EndRender(); //End rendering
        KNR::RenderCommand::Present(); //Present to swapchain
    }

    return 0;
}
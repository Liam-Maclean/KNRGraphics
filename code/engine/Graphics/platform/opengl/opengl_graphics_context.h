#pragma once

#include "graphics/graphics_context.h"

struct GLFWwindow;

namespace QRender
{
    class COpenGLContext :
        public CGraphicsContext
    {
    public:
        COpenGLContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow* window_handle;
    };
}


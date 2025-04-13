// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <GLFW/glfw3.h>

#include "Include/Graphics/Context.h"

namespace Borek {
namespace Graphics {

// OpenGL implementation
class OpenGLContext : public Context {
public:
        OpenGLContext(AbstractWindow* window);
        void Init() override;
        void SwapBuffers() override;

private:
        GLFWwindow* m_Window;
};

}  // namespace Graphics
}  // namespace Borek

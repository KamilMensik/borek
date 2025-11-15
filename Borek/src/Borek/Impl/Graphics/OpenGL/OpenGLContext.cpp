#include "Include/Debug/Log.h"
#include <magic_enum/magic_enum.hpp>
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLContext.h"
#include "Include/Graphics/Backend.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Graphics {

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
        if (type == GL_DEBUG_TYPE_ERROR) {
                BOREK_ENGINE_ERROR("GL ERROR: type = 0x{:x}, severity = 0x{:x}, message = {}\n",
                type, severity, message);
        } else {
                BOREK_ENGINE_WARN("OpenGL: type = 0x{:x}, severity = 0x{:x}, message = {}\n",
                type, severity, message);
        }
}


OpenGLContext::OpenGLContext(AbstractWindow* window)
{
        BOREK_ENGINE_ASSERT(Backend::CheckType(Backend::Type::kOpenGL),
                            "Trying to create an OpenGL context"
                            "when {} backend is selected",
                            magic_enum::enum_name(Backend::GetType()));
        void* w = window->WindowImpl();
        BOREK_ENGINE_ASSERT(window != nullptr,
                            "Window implementation is not initialized");
        m_Window = SCAST<GLFWwindow*>(w);
}

void OpenGLContext::Init()
{
        glfwMakeContextCurrent(m_Window);
        bool result = gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));
        BOREK_ENGINE_ASSERT(result, "Could not initialize glad.");

        BOREK_ENGINE_INFO("OpenGL renderer: {}, OpenGL version: {}",
                          (char*)glGetString(GL_RENDERER),
                          (char*)glGetString(GL_VERSION));

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.199999f);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDebugMessageCallback(MessageCallback, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void OpenGLContext::SwapBuffers()
{
        glfwSwapBuffers(m_Window);
}

}  // Borek
}  // Graphics

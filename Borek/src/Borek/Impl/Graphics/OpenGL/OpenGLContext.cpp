#include <magic_enum/magic_enum.hpp>
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLContext.h"
#include "Include/Graphics/Backend.h"
#include "Include/Core.h"

namespace Borek {
namespace Graphics {

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

        BOREK_ENGINE_INFO("OpenGL renderer: {}",
                          (char*)glGetString(GL_RENDERER));
}

void OpenGLContext::SwapBuffers()
{
        glfwSwapBuffers(m_Window);
}

}  // Borek
}  // Graphics

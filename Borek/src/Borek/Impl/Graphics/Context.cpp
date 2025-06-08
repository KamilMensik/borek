#include "Include/Base/Window.h"
#include "Include/Graphics/Context.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLContext.h"

namespace Borek {
namespace Graphics {

void Context::Create(AbstractWindow* window)
{
        if (s_Instance != nullptr)
                delete s_Instance;

        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                s_Instance = new OpenGLContext(window);
                break;
        }
}

Context* Context::s_Instance = nullptr;

}  // namespace Graphics
}  // namespace Borek

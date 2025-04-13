#include "Include/Graphics/Backend.h"

namespace Borek {
namespace Graphics {

bool Backend::SetType(const std::string& name)
{
        if (name == "OpenGL") {
                s_Type = Type::kOpenGL;
                return true;
        }

        return false;
}

bool Backend::SetType(Type type)
{
        s_Type = type;
        return true;
}

Backend::Type Backend::s_Type = Type::kOpenGL;

}  // namespace Graphics
}  // namespace Borek

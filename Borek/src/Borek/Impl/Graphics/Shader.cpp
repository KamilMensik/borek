// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "Include/Graphics/Shader.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLShader.h"

namespace Borek {
namespace Graphics {

Shader* Shader::Create(const std::string& vertex_source,
                       const std::string& fragment_source)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return new OpenGLShader(vertex_source, fragment_source);
        }
}

}  // namespace Graphics
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/Shader.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLShader.h"

namespace Borek {
namespace Graphics {

Ref<Shader> Shader::Create(const std::string& vertex_source,
                           const std::string& fragment_source)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLShader>(vertex_source,
                                                      fragment_source);
        }
}

Ref<Shader> Shader::Create(const std::string& path)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLShader>(path);
        }
}

}  // namespace Graphics
}  // namespace Borek

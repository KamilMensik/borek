// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLVertexArray.h"
#include "Include/Graphics/VertexArray.h"

namespace Borek {
namespace Graphics {

Ref<VertexArray> VertexArray::Create()
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLVertexArray>();
        }

        return nullptr;
}


}  // namespace Graphics
}  // namespace Borek

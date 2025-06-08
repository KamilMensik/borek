// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/VertexBuffer.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLVertexBuffer.h"

namespace Borek {
namespace Graphics {

Ref<VertexBuffer> VertexBuffer::Create(const void *vertices, uint32_t size,
                                       bool is_dynamic)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLVertexBuffer>(vertices, size, is_dynamic);
        }

        return nullptr;
}

}  // namespace Graphics
}  // namespace Borek

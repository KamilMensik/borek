// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/VertexBuffer.h"
#include "Include/Graphics/Backend.h"

#include "Include/Graphics/OpenGL/OpenGLVertexBuffer.h"

namespace Borek {
namespace Graphics {

VertexBuffer* VertexBuffer::Create(float *vertices, uint32_t size)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return new OpenGLVertexBuffer(vertices, size);
        }

        return nullptr;
}

}  // namespace Graphics
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/Backend.h"

#include "Include/Graphics/OpenGL/OpenGLIndexBuffer.h"

namespace Borek {
namespace Graphics {

IndexBuffer* IndexBuffer::Create(uint32_t *indexes, uint32_t size)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return new OpenGLIndexBuffer(indexes, size);
        }

        return nullptr;
}

}  // namespace Graphics
}  // namespace Borek

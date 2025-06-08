// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLIndexBuffer.h"

namespace Borek {
namespace Graphics {

Ref<IndexBuffer> IndexBuffer::Create(const uint32_t *indexes, uint32_t count,
                                     bool is_dynamic)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLIndexBuffer>(indexes, count, is_dynamic);
        }

        return nullptr;
}

}  // namespace Graphics
}  // namespace Borek

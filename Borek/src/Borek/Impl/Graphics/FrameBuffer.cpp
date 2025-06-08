// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/FrameBuffer.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLFrameBuffer.h"

namespace Borek {
namespace Graphics {

Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSettings &settings)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLFrameBuffer>(settings);
        }

        return nullptr;
}

}  // namespace Graphics
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include "Include/Graphics/Backend.h"
#include "Include/Graphics/OpenGL/OpenGLTexture.h"
#include <cstdint>

namespace Borek {
namespace Graphics {

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLTexture2D>(path);
        default:
                return nullptr;
        }
}

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height,
                                 const uint8_t* data, int channels,
                                 uint32_t flags)
{
        switch (Backend::GetType()) {
        case Backend::Type::kOpenGL:
                return NewRef<OpenGLTexture2D>(width, height, data, channels, flags);
        default:
                return nullptr;
        }
}

}  // namespace Graphics
}  // namespace Borek

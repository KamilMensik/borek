// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include "Include/Graphics/Texture.h"

namespace Borek {
namespace Graphics {

class OpenGLTexture2D : public Texture2D {
public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(uint32_t width, uint32_t height, const uint8_t* data,
                        int channels = 4, uint32_t flags = 0);
        ~OpenGLTexture2D() override;
        void Bind(uint32_t slot = 0) const override;
        uint32_t GetId() const override;

private:
        uint64_t m_BindlessTexHandle;
        uint32_t m_Id;
};

}  // namespace Graphics
}  // namespace Borek

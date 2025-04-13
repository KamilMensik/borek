// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/IndexBuffer.h"

namespace Borek {
namespace Graphics {

class OpenGLIndexBuffer : public IndexBuffer {
public:
        OpenGLIndexBuffer(uint32_t* vertexes, uint32_t count);
        ~OpenGLIndexBuffer() override;
        void Bind() const override;
        void Unbind() const override;

private:
        uint32_t m_Count;
        uint32_t m_Id;
};

}  // namespace Graphics
}  // namespace Borek

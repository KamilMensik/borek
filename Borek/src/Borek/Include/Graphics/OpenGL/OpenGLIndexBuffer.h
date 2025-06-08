// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/IndexBuffer.h"

namespace Borek {
namespace Graphics {

class OpenGLIndexBuffer : public IndexBuffer {
public:
        OpenGLIndexBuffer(const uint32_t* vertexes, uint32_t count,
                          bool is_dynamic = false);
        ~OpenGLIndexBuffer() override;
        void Bind() const override;
        void Unbind() const override;
        void SetData(const void* data, uint32_t size) override;
        uint32_t GetCount() const override;

private:
        uint32_t m_Count;
        uint32_t m_MaxCount;
        uint32_t m_Id;
};

}  // namespace Graphics
}  // namespace Borek

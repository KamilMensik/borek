// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/VertexBuffer.h"

namespace Borek {
namespace Graphics {

class OpenGLVertexBuffer : public VertexBuffer {
public:
        OpenGLVertexBuffer(const void* vertexes, uint32_t size,
                           bool is_dynamic = false);
        ~OpenGLVertexBuffer() override;
        void Bind() override;
        void Unbind() override;
        void SetData(const void* vertexes, uint32_t size) override;
        inline void SetBufferDescriptor(
                const std::initializer_list<BufferDescriptor::Element>& els)
                override
        {
                m_Descriptor = BufferDescriptor(els);
        }

        const BufferDescriptor& GetBufferDescriptor() override
        {
                return m_Descriptor;
        }

private:
        BufferDescriptor m_Descriptor;
        uint32_t m_Id;
        uint32_t m_Size;
};

}  // namespace Graphics
}  // namespace Borek

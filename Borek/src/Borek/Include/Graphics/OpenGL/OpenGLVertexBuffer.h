// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/VertexBuffer.h"

namespace Borek {
namespace Graphics {

class OpenGLVertexBuffer : public VertexBuffer {
public:
        OpenGLVertexBuffer(float* vertexes, uint32_t size);
        ~OpenGLVertexBuffer() override;
        void Bind() override;
        void Unbind() override;
        void SetBufferDescriptor(const BufferDescriptor& desc) override
        {
                m_Descriptor = desc;
        }
        const BufferDescriptor& GetBufferDescriptor() override
        {
                return m_Descriptor;
        }

private:
        BufferDescriptor m_Descriptor;
        uint32_t m_Id;
};

}  // namespace Graphics
}  // namespace Borek

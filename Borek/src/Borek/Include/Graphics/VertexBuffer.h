// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "./BufferDescriptor.h"

namespace Borek {
namespace Graphics {

class VertexBuffer {
public:
        virtual ~VertexBuffer() {}
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetBufferDescriptor(const BufferDescriptor& desc) = 0;
        virtual const BufferDescriptor& GetBufferDescriptor() = 0;

        static VertexBuffer* Create(float* vertexes, uint32_t size);
};

}  // namespace Graphics
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/BufferDescriptor.h"

namespace Borek
{
namespace Graphics
{

class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void SetBufferDescriptor(
                const std::initializer_list<BufferDescriptor::Element>&) = 0;
        virtual const BufferDescriptor &GetBufferDescriptor() = 0;

        static Ref<VertexBuffer> Create(const void *vertexes, uint32_t size,
                                        bool is_dynamic = false);
};

} // namespace Graphics
} // namespace Borek

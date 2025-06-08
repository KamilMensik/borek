// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Core.h"
#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/VertexBuffer.h"

namespace Borek {
namespace Graphics {

class VertexArray {
public:
        virtual ~VertexArray() {};
        virtual void AddVertexBuffer(Ref<VertexBuffer>) = 0;
        virtual void SetIndexBuffer(Ref<IndexBuffer>) = 0;
        virtual uint32_t VertexCount() const = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        static Ref<VertexArray> Create();
};

}  // namespace Graphics
}  // namespace Borek

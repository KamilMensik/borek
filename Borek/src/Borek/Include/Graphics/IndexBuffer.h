// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Core.h"

namespace Borek {
namespace Graphics {

class IndexBuffer {
public:
        virtual ~IndexBuffer() {}
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void SetData(const void* data, uint32_t count) = 0;
        virtual uint32_t GetCount() const = 0;

        static Ref<IndexBuffer> Create(const uint32_t* indexes, uint32_t count,
                                       bool is_dynamic = false);
};

}  // namespace Graphics
}  // namespace Borek

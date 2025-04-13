// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

namespace Borek {
namespace Graphics {

class IndexBuffer {
public:
        virtual ~IndexBuffer() {}
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        static IndexBuffer* Create(uint32_t* indexes, uint32_t size);
};

}  // namespace Graphics
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include "Include/Core.h"

namespace Borek {
namespace Graphics {

class Texture {
public:
        virtual ~Texture() {};
        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetId() const = 0;
};

class Texture2D : public Texture {
public:
        virtual ~Texture2D() {};
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        static Ref<Texture2D> Create(uint32_t width, uint32_t height,
                                     const uint8_t* data);
        static Ref<Texture2D> Create(const std::string& path);

protected:
        uint32_t m_Width;
        uint32_t m_Height;
};

}  // namespace Graphics
}  // namespace Borek

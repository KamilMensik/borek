// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Graphics/Texture.h"
#include <cstdint>

namespace Borek {

class Asset {
public:
        enum Type : uint32_t {
                kImage,
                kScript,
        };

        Asset(unsigned id) : m_Id(id) {}
        Asset() = default;

        Ref<Graphics::Texture2D> Tex() const;
        operator Ref<Graphics::Texture2D>() const
        {
                return Tex();
        }
        inline unsigned Id() const { return m_Id; }
        operator unsigned() const { return Id(); }


private:
        uint32_t m_Id;
};

}  // namespace Borek

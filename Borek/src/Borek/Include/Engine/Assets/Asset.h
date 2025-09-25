// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <cstdio>

#include "Include/Graphics/Texture.h"

namespace Borek {

class Asset {
public:
        enum Type : uint32_t {
                kImage,
                kScript,
        };

        Asset(unsigned id) : m_Id(id) {}
        Asset() : m_Id(UINT32_MAX) {}

        Ref<Graphics::Texture2D> Tex() const;
        operator Ref<Graphics::Texture2D>() const
        {
                return Tex();
        }
        inline unsigned Id() const { return m_Id; }
        operator unsigned() const { return Id(); }

        Type GetAssetType();
        bool IsNil() { return m_Id == UINT32_MAX; }

private:
        uint32_t m_Id;
};

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Core.h"
#include "Include/Objects/SpriteSheet.h"

namespace Borek {

class SubSprite {
public:
        SubSprite(Ref<SpriteSheet> spritesheet, uint32_t row = 0, uint32_t col = 0)
                : m_SpriteSheet(spritesheet), m_Row(row), m_Col(col) {}

        void Bind(uint32_t slot = 0) { m_SpriteSheet->Bind(); }
        inline void SetRow(uint32_t val) { m_Row = val; }
        inline void SetCol(uint32_t val) { m_Col = val; }
        inline void SetRowCol(uint32_t row, uint32_t col)
        {
                SetRow(row);
                SetCol(col);
        }
        inline glm::vec4 GetBounds()
        {
                return m_SpriteSheet->SubTextureCords(m_Row, m_Col);
        }

        inline Ref<Graphics::Texture2D> GetTexture()
        {
                return m_SpriteSheet->GetTexture();
        }

        static inline Ref<SubSprite> Create(Ref<SpriteSheet> ss)
        {
                return NewRef<SubSprite>(ss);
        }

private:
        Ref<SpriteSheet> m_SpriteSheet;
        uint32_t m_Row;
        uint32_t m_Col;
};

}  // namespace Borek

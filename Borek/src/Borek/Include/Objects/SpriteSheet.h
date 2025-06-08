// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include "glm/ext/vector_float4.hpp"

namespace Borek {

class SpriteSheet {
public:
        SpriteSheet(Ref<Graphics::Texture2D> tex, uint32_t step_x = 32,
                    uint32_t step_y = 32)
                : m_Texture(tex), m_StepX(step_x), m_StepY(step_y) {}

        inline void Bind(uint32_t slot = 0) const { m_Texture->Bind(slot); }
        inline uint32_t GetWidth() const { return m_Texture->GetWidth(); }
        inline uint32_t GetHeight() const { return m_Texture->GetHeight(); }
        inline uint32_t GetId() const { return m_Texture->GetId(); }
        inline glm::vec4 SubTextureCords(uint32_t row, uint32_t col)
        {
                return glm::vec4(row * (float)(m_StepX + m_Gap) / GetWidth(),
                                 col * (float)(m_StepY + m_Gap) / GetHeight(),
                                 ((row + 1) * (float)(m_StepX + m_Gap) - m_Gap) / GetWidth(),
                                 ((col + 1) * (float)(m_StepY + m_Gap) - m_Gap) / GetHeight());
        }
        inline void SetStepX(uint32_t val) { m_StepX = val; }
        inline void SetStepY(uint32_t val) { m_StepY = val; }
        inline void SetStep(uint32_t x, uint32_t y)
        {
                SetStepX(x);
                SetStepY(y);
        }
        inline void SetGap(uint32_t val) { m_Gap = val; }
        inline static Ref<SpriteSheet> Create(const std::string& path)
        {
                return NewRef<SpriteSheet>(Graphics::Texture2D::Create(path));
        }

        inline Ref<Graphics::Texture2D> GetTexture()
        {
                return m_Texture;
        }

private:
        Ref<Graphics::Texture2D> m_Texture;
        uint32_t m_StepX;
        uint32_t m_StepY;
        uint32_t m_Gap;
};

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

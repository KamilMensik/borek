// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>
#include <string>
#include <glm/ext/vector_float4.hpp>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

class SpriteSheet {
public:
        SpriteSheet() : m_StepX(32), m_StepY(32) {}
        SpriteSheet(Ref<Graphics::Texture2D> tex, uint32_t step_x = 32,
                    uint32_t step_y = 32)
                : m_Texture(tex), m_StepX(step_x), m_StepY(step_y) {}

        inline void Bind(uint32_t slot = 0) const { m_Texture->Bind(slot); }
        inline uint32_t GetWidth() const { return m_Texture->GetWidth(); }
        inline uint32_t GetHeight() const { return m_Texture->GetHeight(); }
        uint32_t GetCols() const
        {
                uint32_t cols_pre = m_Texture->GetWidth() / m_StepX;
                return (m_Texture->GetWidth() - (cols_pre - 1) * m_Gap) / m_StepX;
        }
        uint32_t GetRows() const
        {
                uint32_t rows_pre = m_Texture->GetHeight() / m_StepY;
                return (m_Texture->GetHeight() - (rows_pre - 1) * m_Gap) / m_StepY;
        }
        inline uint32_t GetId() const { return m_Texture->GetId(); }
        inline glm::vec4 SubTextureCords(uint32_t row, uint32_t col)
        {
                return glm::vec4(row * (float)(m_StepX + m_Gap) / GetWidth(),
                                 col * (float)(m_StepY + m_Gap) / GetHeight(),
                                 ((row + 1) * (float)(m_StepX + m_Gap) - m_Gap) / GetWidth(),
                                 ((col + 1) * (float)(m_StepY + m_Gap) - m_Gap) / GetHeight());
        }
        inline uint32_t GetStepX() { return m_StepX; }
        inline uint32_t GetStepY() { return m_StepY; }
        inline void SetStepX(uint32_t val) { m_StepX = val; }
        inline void SetStepY(uint32_t val) { m_StepY = val; }
        inline void SetStep(uint32_t x, uint32_t y)
        {
                SetStepX(x);
                SetStepY(y);
        }
        inline uint32_t GetGap() { return m_Gap; }
        inline void SetGap(uint32_t val) { m_Gap = val; }
        inline static Ref<SpriteSheet> Create(const std::string& path)
        {
                return NewRef<SpriteSheet>(Graphics::Texture2D::Create(path));
        }

        inline Ref<Graphics::Texture2D> GetTexture()
        {
                return m_Texture;
        }
        inline void SetTexture(Ref<Graphics::Texture2D> tex)
        {
                m_Texture = tex;
        }
        inline std::string& GetTexturePath()
        {
                return m_TexturePath;
        }

private:
        std::string m_TexturePath;
        Ref<Graphics::Texture2D> m_Texture;
        uint32_t m_StepX;
        uint32_t m_StepY;
        uint32_t m_Gap = 0;
};

}  // namespace Borek

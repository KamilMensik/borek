// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Core.h"
#include "Include/Objects/SpriteSheet.h"

namespace Borek {

class Tilemap {
public:
        Tilemap(Ref<SpriteSheet> spritesheet) : m_SpriteSheet(spritesheet) {}

        void Bind(uint32_t slot = 0) { m_SpriteSheet->Bind(); }
        inline glm::vec4 GetBounds()
        {
                return glm::vec4();
                //return m_SpriteSheet->SubTextureCords(m_Row, m_Col);
        }

        inline Ref<Graphics::Texture2D> GetTexture()
        {
                return m_SpriteSheet->GetTexture();
        }

        static inline Ref<Tilemap> Create(Ref<SpriteSheet> ss)
        {
                return NewRef<Tilemap>(ss);
        }

private:
        Ref<SpriteSheet> m_SpriteSheet;
};

}  // namespace Borek

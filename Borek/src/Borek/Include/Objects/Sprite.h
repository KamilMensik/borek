// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

class Sprite {
public:
        Sprite(Ref<Graphics::Texture2D> tex) : m_Texture(tex) {}
        inline void Bind(uint32_t slot = 0) const { m_Texture->Bind(slot); }
        inline uint32_t GetWidth() const { return m_Texture->GetWidth(); }
        inline uint32_t GetHeight() const { return m_Texture->GetHeight(); }
        inline uint32_t GetId() const { return m_Texture->GetId(); }
        const Ref<Graphics::Texture2D>& GetTexture() { return m_Texture; }
        inline static Ref<Sprite> Create(const std::string& path)
        {
                return NewRef<Sprite>(Graphics::Texture2D::Create(path));
        }

private:
        Ref<Graphics::Texture2D> m_Texture;
};

}  // namespace Borek

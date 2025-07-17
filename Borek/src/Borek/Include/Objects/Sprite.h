// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>
#include <cstdint>

#include "Include/Core.h"
#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/Assets/Asset.h"

namespace Borek {

class Sprite {
friend class SceneSerializer;
public:
        Sprite(Asset tex) : m_Texture(tex) {}
        inline void Bind(uint32_t slot = 0) const { m_Texture.Tex()->Bind(slot); }
        inline uint32_t GetWidth() const { return m_Texture.Tex()->GetWidth(); }
        inline uint32_t GetHeight() const { return m_Texture.Tex()->GetHeight(); }
        inline uint32_t GetId() const { return m_Texture.Tex()->GetId(); }
        inline Asset GetAsset() const { return m_Texture; }
        const Ref<Graphics::Texture2D> GetTexture() { return m_Texture; }
        inline static Ref<Sprite> Create(const std::string& path)
        {
                return NewRef<Sprite>(AssetManager::GetTexture(path));
        }

private:
        Asset m_Texture;
};

}  // namespace Borek

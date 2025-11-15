// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>
#include <cstdint>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/TexAsset.h"

namespace Borek {

class Sprite {
friend class SceneSerializer;
public:
        Sprite(Asset<TexAsset> tex) : m_Texture(tex) {}
        inline void Bind(uint32_t slot = 0) const { GetTexture()->Bind(slot); }
        inline uint32_t GetWidth() const { return GetTexture()->GetWidth(); }
        inline uint32_t GetHeight() const { return GetTexture()->GetHeight(); }
        inline uint32_t GetId() const { return GetTexture()->GetId(); }
        inline Asset<TexAsset> GetAsset() const { return m_Texture; }
        const Ref<Graphics::Texture2D> GetTexture() const { return m_Texture.Convert(); }
        inline static Ref<Sprite> Create(const std::string& path)
        {
                return NewRef<Sprite>(AssetManager::Get<TexAsset>(path));
        }

private:
        Asset<TexAsset> m_Texture;
};

}  // namespace Borek

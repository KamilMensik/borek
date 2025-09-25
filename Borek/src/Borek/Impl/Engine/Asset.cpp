// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/AssetManager.h"

namespace Borek {

Ref<Graphics::Texture2D> Asset::Tex() const
{
        return AssetManager::GetTexture(m_Id);
}

Asset::Type Asset::GetAssetType()
{
        return AssetManager::GetAssetType(m_Id);
}

}  // namespace Borek

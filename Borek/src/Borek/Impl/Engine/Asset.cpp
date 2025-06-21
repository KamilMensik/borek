// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Asset.h"
#include "Include/Engine/AssetManager.h"

namespace Borek {

Ref<Graphics::Texture2D> Asset::Tex() const
{
        return AssetManager::GetTexture(m_Id);
}

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>

#include "Include/Core.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

struct TexAsset : public IAsset {
        ~TexAsset() override;

        void
        Serialize(const std::filesystem::path &path) override;

        void
        Deserialize(const std::filesystem::path &path) override;

        void
        Load() override;

        void
        Unload() override;

        uint32_t
        GetType() const override;

        operator Ref<Graphics::Texture2D>();

        Ref<Graphics::Texture2D> texture;
        uint8_t* data = nullptr;
        int width, height, channels;
        uint32_t flags = 0;
};


}  // namespace Borek

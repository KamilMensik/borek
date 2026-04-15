// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <flat_map>

#include "Include/Base/Symbol.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"

namespace Borek {

struct AnimationAsset : public IAsset {
        ~AnimationAsset() override = default;

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

        std::vector<uint32_t> animation_frames;
        std::flat_map<Symbol, uint32_t> animation_by_name;
        Asset<SpriteSheetAsset> sprite_sheet;
        uint32_t flags = 0;
};

}  // namespace Borek

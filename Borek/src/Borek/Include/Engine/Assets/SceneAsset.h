// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/IAsset.h"

namespace Borek {

struct SceneAsset : public IAsset {
        ~SceneAsset() override = default;

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

        std::string scene_path;
};

}  // namespace Borek

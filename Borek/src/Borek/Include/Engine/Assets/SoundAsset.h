// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "Include/Engine/Assets/IAsset.h"
#include "Include/Core.h"

namespace Borek {

struct SoundAsset : public IAsset {
        ~SoundAsset() override;

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

        std::string filepath;
        BitFlags flags = 0;
        float volume = 1.0f;
};


}  // namespace Borek

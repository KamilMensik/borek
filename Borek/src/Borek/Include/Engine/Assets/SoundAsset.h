// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include <cstdint>
#include <filesystem>

#include <miniaudio.h>

#include "Include/Engine/Assets/IAsset.h"
#include <string>

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

        void
        Play();

        void
        Stop();

        uint32_t
        GetType() const override;

        ma_sound sound;
        std::string filepath;
        BitFlags flags = 0;
        uint32_t engine_generation = UINT32_MAX;
        float volume = 1.0f;
        bool is_playing = false;
        bool is_loaded = false;
};


}  // namespace Borek

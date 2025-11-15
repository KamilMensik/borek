// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SoundAsset.h"

namespace Borek {

enum SoundPlayerFlags {
        SoundPlayerFlags_Playing = 1,
        SoundPlayerFlags_Autoplay = 2,
};

struct SoundPlayerComponent {
        Asset<SoundAsset> sound;
        BitFlags flags;

        ~SoundPlayerComponent();
        SoundPlayerComponent() = default;
        SoundPlayerComponent(SoundPlayerComponent&& other);

        void
        Play();

        void
        Stop();
};

}  // namespace Borek

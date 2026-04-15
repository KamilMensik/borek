// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <miniaudio.h>

#include "Include/Base/MsgConnection.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Alloc.h"

namespace Borek {

enum SoundPlayerFlags {
        SoundPlayerFlags_None = 0,
        SoundPlayerFlags_Playing = 1,
        SoundPlayerFlags_Autoplay = 2,
};

struct SoundPlayerComponent {
        Asset<SoundAsset> asset;
        BitFlags flags;
        AUniq<MsgConnection> on_finished;
        Uniq<ma_sound> sound;

        SoundPlayerComponent();
        ~SoundPlayerComponent();
        SoundPlayerComponent(const SoundPlayerComponent& other);
        SoundPlayerComponent(SoundPlayerComponent&& other);

        SoundPlayerComponent&
        operator =(const SoundPlayerComponent& other);

        SoundPlayerComponent&
        operator =(SoundPlayerComponent&& other);

        void
        Play();

        void
        Stop();

        void
        Switch(const Asset<SoundAsset> asset);

        bool
        Finished();
};

}  // namespace Borek

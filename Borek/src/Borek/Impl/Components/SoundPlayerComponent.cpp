// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/SoundplayerComponent.h"
#include "Include/Debug/Log.h"

namespace Borek {

SoundPlayerComponent::~SoundPlayerComponent()
{
        if (flags.HasFlags(SoundPlayerFlags_Playing))
                Stop();

        flags.SetFlags(SoundPlayerFlags_Playing, false);
}

SoundPlayerComponent::SoundPlayerComponent(SoundPlayerComponent&& other)
{
        flags = other.flags;
        sound = std::move(other.sound);
}

void
SoundPlayerComponent::Play()
{
        ma_sound_seek_to_second(&sound->sound, 0.0f);
        sound->Play();
}

void
SoundPlayerComponent::Stop()
{
        sound->Stop();
}

}  // namespace Borek

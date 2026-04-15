// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/SoundplayerComponent.h"
#include "Include/Base/MsgConnection.h"
#include "Include/Base/Sound.h"
#include "Include/Engine/Alloc.h"

namespace Borek {

SoundPlayerComponent::SoundPlayerComponent()
{
        on_finished = NewAUniq<MsgConnection>();
}

SoundPlayerComponent::~SoundPlayerComponent()
{
        if (!asset.IsValid())
                return;

        if (flags.HasFlags(SoundPlayerFlags_Playing))
                Stop();

        ma_sound_uninit(sound.get());
}
SoundPlayerComponent::SoundPlayerComponent(const SoundPlayerComponent& other)
{
        flags = other.flags;
        asset = other.asset;
        if (other.sound) {
                sound = NewUniq<ma_sound>();
                *sound = *other.sound;
        }
        on_finished = NewAUniq<MsgConnection>();
        *on_finished = *other.on_finished;
}

SoundPlayerComponent::SoundPlayerComponent(SoundPlayerComponent&& other)
{
        flags = other.flags;
        asset = std::move(other.asset);
        sound = std::move(other.sound);
        on_finished = std::move(other.on_finished);
}

SoundPlayerComponent&
SoundPlayerComponent::operator =(const SoundPlayerComponent& other)
{
        flags = other.flags;
        if (other.sound) {
                sound = NewUniq<ma_sound>();
                *sound = *other.sound;
        }
        asset = other.asset;
        on_finished = NewAUniq<MsgConnection>();
        *on_finished = *other.on_finished;
        return *this;
}

SoundPlayerComponent&
SoundPlayerComponent::operator =(SoundPlayerComponent&& other)
{
        flags = other.flags;
        sound = std::move(other.sound);
        asset = std::move(other.asset);
        on_finished = std::move(other.on_finished);
        return *this;
}

void
SoundPlayerComponent::Play()
{
        if (!asset.IsValid())
                return;

        if (!sound) {
                sound = NewUniq<ma_sound>();
                ma_sound_init_from_file(&SoundEngine::Get(),
                                        asset->filepath.c_str(),
                                        asset->flags, NULL, NULL, sound.get());
                ma_sound_set_volume(sound.get(), asset->volume);
        }

        flags.SetFlags(SoundPlayerFlags_Playing, true);
        ma_sound_seek_to_second(sound.get(), 0.0f);
        ma_sound_start(sound.get());
}

void
SoundPlayerComponent::Stop()
{
        if (!(flags & SoundPlayerFlags_Playing))
                return;

        flags.SetFlags(SoundPlayerFlags_Playing, false);
        ma_sound_stop(sound.get());
}

void
SoundPlayerComponent::Switch(const Asset<SoundAsset> asset)
{
        Stop();
        ma_sound_uninit(sound.get());
        sound = nullptr;
        this->asset = asset;
}

bool
SoundPlayerComponent::Finished()
{
        if (!(flags & SoundPlayerFlags_Playing && ma_sound_at_end(sound.get())))
                return false;
        
        flags.SetFlags(SoundPlayerFlags_Playing, false);
        return true;
}

}  // namespace Borek

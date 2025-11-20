// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Sound.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/AssetFlags.h"
#include <cstdint>
#include <filesystem>
#include <fstream>

#include <miniaudio.h>

#include "Include/Engine/Assets/SoundAsset.h"

namespace Borek {

namespace fs = std::filesystem;

SoundAsset::~SoundAsset()
{
        Unload();
}

void
SoundAsset::Serialize(const fs::path &path)
{
        std::ofstream asset(path, std::ios::binary);
        const int filepath_size = filepath.size();

        asset.write(RCAST<const char*>(&flags.val), sizeof(uint32_t))
             .write(RCAST<const char*>(&volume), sizeof(float))
             .write(RCAST<const char*>(&filepath_size), sizeof(int))
             .write(filepath.c_str(), filepath.size());
}

void
SoundAsset::Deserialize(const fs::path &path)
{
        std::ifstream asset(path, std::ios::binary);
        int filepath_size = filepath.size();

        asset.read(RCAST<char*>(&flags.val), sizeof(uint32_t))
             .read(RCAST<char*>(&volume), sizeof(float))
             .read(RCAST<char*>(&filepath_size), sizeof(int));

        filepath.resize(filepath_size + 1);
        asset.read(filepath.data(), filepath_size);
}

void
SoundAsset::Load()
{
        if (is_loaded)
                Unload();

        engine_generation = SoundEngine::GetGeneration();
        BitFlags sound_flags;
        sound_flags.SetFlags(MA_SOUND_FLAG_STREAM, flags.HasFlags(SoundFlags_Stream));
        sound_flags.SetFlags(MA_SOUND_FLAG_LOOPING, flags.HasFlags(SoundFlags_Looping));

        BOREK_ENGINE_INFO("Loading sound!");
        ma_sound_init_from_file(&SoundEngine::Get(), filepath.c_str(),
                                sound_flags, NULL, NULL, &sound);
        ma_sound_set_volume(&sound, volume);

        is_loaded = true;
}

void
SoundAsset::Unload()
{
        BOREK_ENGINE_INFO("Got here");
        if (!SoundEngine::IsSoundValid(*this))
                return;

        BOREK_ENGINE_INFO("Got here 2");
        Stop();
        BOREK_ENGINE_INFO("Is loaded? {}", is_loaded);
        if (is_loaded)
                ma_sound_uninit(&sound);

        is_loaded = false;
}

void
SoundAsset::Play()
{
        ma_sound_start(&sound);
        is_playing = true;
}

void
SoundAsset::Stop()
{
        if (is_playing)
                ma_sound_stop(&sound);

        is_playing = false;
}

uint32_t
SoundAsset::GetType() const
{
        return AssetType_Sound;
}


}  // namespace Borek

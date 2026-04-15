// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <filesystem>
#include <fstream>

#include <miniaudio.h>

#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/AssetFlags.h"

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
        BitFlags sound_flags;
        sound_flags.SetFlags(MA_SOUND_FLAG_STREAM, flags.HasFlags(SoundFlags_Stream));
        sound_flags.SetFlags(MA_SOUND_FLAG_LOOPING, flags.HasFlags(SoundFlags_Looping));
}

void
SoundAsset::Unload()
{
}

uint32_t
SoundAsset::GetType() const
{
        return AssetType_Sound;
}


}  // namespace Borek

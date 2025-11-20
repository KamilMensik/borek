// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/ResourceAssetifier.h"
#include <cstdint>
#include <cstring>
#include <fstream>

#include <stb/image.h>

#include "Include/Engine/Assets/IAsset.h"
#include "Include/Debug/Assert.h"
#include "Include/Engine/Assets/TexAsset.h"
#include "Include/Graphics/Texture.h"

namespace fs = std::filesystem;

namespace Borek {

TexAsset::~TexAsset()
{
        if (data)
                delete[] data;
}

void
TexAsset::Serialize(const fs::path &path)
{
        std::ofstream asset(path, std::ios::binary);

        asset.write(RCAST<const char*>(&width), sizeof(int))
             .write(RCAST<const char*>(&height), sizeof(int))
             .write(RCAST<const char*>(&channels), sizeof(int))
             .write(RCAST<const char*>(&flags), sizeof(uint32_t))
             .write(RCAST<char*>(data), width * height * channels);
}

void
TexAsset::Deserialize(const fs::path &path)
{
        std::ifstream asset;
        if (path.extension() != ".tex") {
                ResourceAssetifier::AssetifyFile(path);
                fs::path asset_path = path;
                asset_path.replace_extension(".tex");
                asset = std::ifstream(asset_path, std::ios::binary);
        } else {
                asset = std::ifstream(path, std::ios::binary);
        }

        Unload();

        asset.read(RCAST<char*>(&width), sizeof(int))
             .read(RCAST<char*>(&height), sizeof(int))
             .read(RCAST<char*>(&channels), sizeof(int))
             .read(RCAST<char*>(&flags), sizeof(int));

        if (data)
                delete[] data;

        data = new uint8_t[width * height * channels];

        asset.read(RCAST<char*>(data), width * height * channels);
}

void
TexAsset::Load()
{
        texture = Graphics::Texture2D::Create(width, height, data, channels, flags);
}

void
TexAsset::Unload()
{
        texture = nullptr;
}

uint32_t
TexAsset::GetType() const
{
        return AssetType_Tex;
}

TexAsset::operator Ref<Graphics::Texture2D>()
{
        return texture;
}


}  // namespace Borek

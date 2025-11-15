// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "glm/ext/vector_float2.hpp"
#include <cstdint>
#include <fstream>

#include <stb/image.h>

#include "Include/Engine/Assets/IAsset.h"
#include "Include/Debug/Assert.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Graphics/Texture.h"

namespace fs = std::filesystem;

namespace Borek {

SpriteSheetAsset::~SpriteSheetAsset()
{
        if (data)
                delete data;
}

void
SpriteSheetAsset::Serialize(const fs::path &path)
{
        std::ofstream asset(path, std::ios::binary);

        asset.write(RCAST<const char*>(&width), sizeof(int))
             .write(RCAST<const char*>(&height), sizeof(int))
             .write(RCAST<const char*>(&channels), sizeof(int))
             .write(RCAST<const char*>(&flags), sizeof(uint32_t))
             .write(RCAST<const char*>(&step_x), sizeof(uint32_t))
             .write(RCAST<const char*>(&step_y), sizeof(uint32_t))
             .write(RCAST<const char*>(&gap_x), sizeof(uint32_t))
             .write(RCAST<const char*>(&gap_y), sizeof(uint32_t))
             .write(RCAST<char*>(data), width * height * channels);
}

void
SpriteSheetAsset::Deserialize(const fs::path &path)
{
        std::ifstream asset(path, std::ios::binary);
        Unload();

        asset.read(RCAST<char*>(&width), sizeof(int))
             .read(RCAST<char*>(&height), sizeof(int))
             .read(RCAST<char*>(&channels), sizeof(int))
             .read(RCAST<char*>(&flags), sizeof(uint32_t))
             .read(RCAST<char*>(&step_x), sizeof(uint32_t))
             .read(RCAST<char*>(&step_y), sizeof(uint32_t))
             .read(RCAST<char*>(&gap_x), sizeof(uint32_t))
             .read(RCAST<char*>(&gap_y), sizeof(uint32_t));

        if (data)
                delete[] data;

        data = new uint8_t[width * height * channels];

        asset.read(RCAST<char*>(data), width * height * channels);
}

void
SpriteSheetAsset::Load()
{
        texture = Graphics::Texture2D::Create(width, height, data, channels);
}

void
SpriteSheetAsset::Unload()
{
        texture = nullptr;
}

uint32_t
SpriteSheetAsset::GetType() const
{
        return AssetType_SpriteSheet;
}

uint32_t
SpriteSheetAsset::GetRows() const
{

        return (height + gap_y) / (step_y + gap_y);
}

uint32_t
SpriteSheetAsset::GetCols() const
{
        return (width + gap_x) / (step_x + gap_x);
}

uint32_t
SpriteSheetAsset::GetCells() const
{
        return GetRows() * GetCols();
}

glm::vec4
SpriteSheetAsset::SubTextureCords(uint32_t row, uint32_t col) const
{
        const glm::vec2 pos_min(col * (step_x + gap_x), row * (step_y + gap_y));
        const glm::vec2 pos_max = pos_min + glm::vec2(step_x, step_y);

        return glm::vec4(pos_min, pos_max) / glm::vec4(width, height, width, height);
}

SpriteSheetAsset::operator Ref<Graphics::Texture2D>()
{
        return texture;
}


}  // namespace Borek

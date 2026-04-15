// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <fstream>
#include <string>

#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/AnimationAsset.h"

namespace Borek {

void
AnimationAsset::Serialize(const std::filesystem::path &path)
{
        std::ofstream asset(path, std::ios::binary);

        const int animations_size = animation_frames.size();
        const int animations_count = animation_by_name.size();
        asset.write(RCAST<const char*>(&flags), sizeof(uint32_t))
             .write(RCAST<const char*>(&animations_size), sizeof(int))
             .write(RCAST<const char*>(animation_frames.data()),
                    sizeof(uint32_t) * animations_size)
             .write(RCAST<const char*>(&animations_count), sizeof(int));

        for (auto it : animation_by_name) {
                const int name_size = it.first.Str().size();
                asset.write(RCAST<const char*>(&name_size), sizeof(int))
                     .write(it.first.Str().data(), name_size)
                     .write(RCAST<const char*>(&it.second), sizeof(uint32_t));
        }

        if (sprite_sheet.IsValid()) {
                const uint32_t path_size = sprite_sheet.GetPath().size();
                asset.write(RCAST<const char*>(&path_size), sizeof(uint32_t))
                     .write(sprite_sheet.GetPath().c_str(), path_size);
        } else {
                const uint32_t path_size = 0;
                asset.write(RCAST<const char*>(&path_size), sizeof(uint32_t));
        }
}

void
AnimationAsset::Deserialize(const std::filesystem::path &path)
{
        std::ifstream asset(path, std::ios::binary);

        int animations_size;
        asset.read(RCAST<char*>(&flags), sizeof(uint32_t))
             .read(RCAST<char*>(&animations_size), sizeof(int));

        animation_frames.resize(animations_size);
        int animations_count;

        asset.read(RCAST<char*>(animation_frames.data()),
                   sizeof(uint32_t) * animations_size)
             .read(RCAST<char*>(&animations_count), sizeof(int));

        std::string name;
        for (int i = 0; i < animations_count; i++) {
                int name_size;
                uint32_t start_frame;

                asset.read(RCAST<char*>(&name_size), sizeof(int));
                name.resize(name_size);

                asset.read(name.data(), name_size)
                     .read(RCAST<char*>(&start_frame), sizeof(uint32_t));

                animation_by_name[Symbol(name)] = start_frame;
        }

        uint32_t path_size;
        asset.read(RCAST<char*>(&path_size), sizeof(uint32_t));

        if (path_size > 0) {
                char* cpath = new char[path_size + 1];

                asset.read(cpath, path_size);
                cpath[path_size] = 0;

                sprite_sheet = AssetManager::Get<SpriteSheetAsset>(cpath);
                delete[] cpath;
        }
}

void
AnimationAsset::Load()
{
}

void
AnimationAsset::Unload()
{
}

uint32_t
AnimationAsset::GetType() const
{
        return AssetType_Animation;
}

}  // namespace Borek

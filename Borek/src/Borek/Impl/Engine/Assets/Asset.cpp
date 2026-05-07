// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "Include/Engine/Assets/AnimationAsset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/TexAsset.h"
#include "Include/Engine/Assets/TilemapAsset.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Engine/Utils/StringHelpers.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/IAsset.h"

namespace Borek {

uint32_t
AssetManager::GetRaw(std::string_view path)
{
        if (path.empty())
                return UINT32_MAX;

        if (auto res = s_AssetByPath.find(path); res != s_AssetByPath.end()) {
                Increment(res->second);
                return res->second;
        }

        std::filesystem::path absolute = Utils::Path::FromRelative(path);
        if (!std::filesystem::exists(absolute))
                ResourceAssetifier::AssetifyFolder(absolute.parent_path());

        if (!std::filesystem::exists(absolute))
                return UINT32_MAX;

        uint32_t slot;
        if (s_FirstFreeSlot != UINT32_MAX) {
                slot = s_FirstFreeSlot;
                s_FirstFreeSlot = s_AssetTypes[s_FirstFreeSlot];

                s_AssetPaths[slot] = path;
                s_AssetReferences[slot] = 0;
        } else {
                slot = s_Assets.size();
                s_Assets.emplace_back(nullptr);
                s_AssetPaths.emplace_back(path);
                s_AssetReferences.emplace_back(0);
                s_AssetTypes.emplace_back(0);
        }

        s_AssetByPath[std::string(path)] = slot;
        BOREK_ENGINE_INFO("Extension: {}", absolute.extension().string());
        switch (HashP(absolute.extension())) {
        case Hash(".tex"):
                s_Assets[slot] = NewUniq<TexAsset>();
                break;
        case Hash(".scr"):
                s_Assets[slot] = NewUniq<ScriptAsset>();
                break;
        case Hash(".sst"):
                s_Assets[slot] = NewUniq<SpriteSheetAsset>();
                break;
        case Hash(".tmap"):
                s_Assets[slot] = NewUniq<TilemapAsset>();
                break;
        case Hash(".snd"):
                s_Assets[slot] = NewUniq<SoundAsset>();
                break;
        case Hash(".anim"):
                s_Assets[slot] = NewUniq<AnimationAsset>();
                break;
        case Hash(".scn"):
                s_Assets[slot] = NewUniq<SceneAsset>();
                break;
        default:
                BOREK_ENGINE_ASSERT(false, "Should not happen!");
        }
        s_Assets[slot]->Deserialize(absolute);
        s_AssetTypes[slot] = s_Assets[slot]->GetType();
        Increment(slot);
        return slot;
}

void
AssetManager::Refresh(std::string_view path, Uniq<IAsset> asset)
{
        auto it = s_AssetByPath.find(path);
        if (it == s_AssetByPath.end()) {
                return;
        }

        s_AssetTypes[it->second] = asset->GetType();
        s_Assets[it->second] = std::move(asset);
        if (s_AssetReferences[it->second] > 0)
                s_Assets[it->second]->Load();
}

void
AssetManager::RefreshPath(std::string_view from, std::string_view to)
{
        auto it = s_AssetByPath.find(from);
        if (it == s_AssetByPath.end()) {
                return;
        }

        uint32_t asset_index = it->second;
        s_AssetByPath.erase(it);
        s_AssetByPath[std::string(to)] = asset_index;
        s_AssetPaths[asset_index] = to;
}

uint32_t
AssetManager::GetAssetType(uint32_t id)
{
        return s_AssetTypes[id];
}

const std::string&
AssetManager::GetPath(uint32_t id)
{
        return s_AssetPaths[id];
}

void
AssetManager::Clean(bool force)
{
        for (int i = 0; int refcount : s_AssetReferences) {
                if (force || refcount <= 0) {
                        s_Assets[i] = nullptr;
                        s_AssetTypes[i] = s_FirstFreeSlot;
                        s_FirstFreeSlot = i;
                        s_AssetByPath.erase(s_AssetPaths[i]);
                }

                i++;
        }
}

void
AssetManager::Increment(uint32_t asset_id)
{
        if (s_AssetReferences[asset_id] == 0) {
                s_Assets[asset_id]->Load();
                BOREK_ENGINE_INFO("|AssetManager| - Loaded Asset {}",
                                   s_AssetPaths[asset_id]);
        }

        s_AssetReferences[asset_id]++;
}

void
AssetManager::Decrement(uint32_t asset_id)
{
        if (--s_AssetReferences[asset_id] <= 0) {
                s_Assets[asset_id]->Unload();
                BOREK_ENGINE_INFO("|AssetManager| - Unloaded Asset {}",
                                   s_AssetPaths[asset_id]);
        }
}

void
AssetManager::Remove(uint32_t id)
{
        const std::string& path = s_AssetPaths[id];
        s_AssetByPath.erase(path);
        s_Assets[id].reset();
        s_AssetTypes[id] = s_FirstFreeSlot;
        s_FirstFreeSlot = id;
}

void
AssetManager::Remove(std::string_view path)
{
        auto it = s_AssetByPath.find(path);
        if (it == s_AssetByPath.end())
                return;

        s_Assets[it->second].reset();
        s_AssetTypes[it->second] = s_FirstFreeSlot;
        s_FirstFreeSlot = it->second;
        s_AssetByPath.erase(std::string(path));
}

std::vector<Uniq<IAsset>> AssetManager::s_Assets;
std::vector<int> AssetManager::s_AssetReferences;
std::vector<uint32_t> AssetManager::s_AssetTypes;
std::vector<std::string> AssetManager::s_AssetPaths;
StringMap<uint32_t> AssetManager::s_AssetByPath;
uint32_t AssetManager::s_FirstFreeSlot = UINT32_MAX;

}  // namespace Borek

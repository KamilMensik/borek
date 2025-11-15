// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/IAsset.h"

namespace Borek {

void
AssetManager::Refresh(const std::string& path, Uniq<IAsset> asset)
{
        auto it = s_AssetByPath.find(path);
        if (it == s_AssetByPath.end()) {
                return;
        }

        if (s_AssetReferences[it->second] > 0)
                asset->Load();

        s_AssetTypes[it->second] = asset->GetType();
        s_Assets[it->second] = std::move(asset);
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
AssetManager::Clean()
{
        for (int i = 0; int refcount : s_AssetReferences) {
                if (refcount <= 0) {
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
                BOREK_ENGINE_INFO("|AssetManager| - Loaded Asset {}",
                                   s_AssetPaths[asset_id]);
                s_Assets[asset_id]->Load();
        }

        s_AssetReferences[asset_id]++;
}

void
AssetManager::Decrement(uint32_t asset_id)
{
        if (--s_AssetReferences[asset_id] <= 0) {
                BOREK_ENGINE_INFO("|AssetManager| - Unloaded Asset {}",
                                   s_AssetPaths[asset_id]);
                s_Assets[asset_id]->Unload();
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
AssetManager::Remove(const std::string& path)
{
        auto it = s_AssetByPath.find(path);
        if (it == s_AssetByPath.end())
                return;

        s_Assets[it->second].reset();
        s_AssetTypes[it->second] = s_FirstFreeSlot;
        s_FirstFreeSlot = it->second;
        s_AssetByPath.erase(path);
}

std::vector<Uniq<IAsset>> AssetManager::s_Assets;
std::vector<int> AssetManager::s_AssetReferences;
std::vector<uint32_t> AssetManager::s_AssetTypes;
std::vector<std::string> AssetManager::s_AssetPaths;
std::unordered_map<std::string, uint32_t> AssetManager::s_AssetByPath;
uint32_t AssetManager::s_FirstFreeSlot = UINT32_MAX;

}  // namespace Borek

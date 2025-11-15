// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/ResourceAssetifier.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include <concepts>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "Include/Core.h"
#include "Include/Engine/Assets/IAsset.h"

namespace Borek {

template <class T> requires std::derived_from<T, IAsset>
class Asset {
public:
        Asset();
        Asset(uint32_t id);
        Asset(const Asset<T>& asset);
        Asset(const Asset<T>&& asset);
        ~Asset();

        uint32_t
        GetId() const;

        const std::string&
        GetPath() const;

        T&
        Convert() const;

        bool
        IsValid() const;

        operator uint32_t() const;
        operator T&() const;
        T* operator ->() const;
        Asset& operator=(const Asset& asset);
        Asset& operator=(Asset&& asset);

private:
        uint32_t m_Id;
};

class AssetManager {
friend class ResourceAssetifier;
public:
        template <class T> requires std::derived_from<T, IAsset>
        static Asset<T>
        Get(const std::string& path);

        static void
        Refresh(const std::string& path, Uniq<IAsset> asset);

        template <class T> requires std::derived_from<T, IAsset>
        static T&
        GetData(uint32_t id);

        static uint32_t
        GetAssetType(uint32_t id);

        static const std::string&
        GetPath(uint32_t id);

        static void
        Clean();

        static void
        Increment(uint32_t asset_id);

        static void
        Decrement(uint32_t asset_id);

        static void
        Remove(uint32_t id);

        static void
        Remove(const std::string& path);

private:
        static std::vector<Uniq<IAsset>> s_Assets;
        static std::vector<int> s_AssetReferences;
        static std::vector<uint32_t> s_AssetTypes;
        static std::vector<std::string> s_AssetPaths;
        static std::unordered_map<std::string, uint32_t> s_AssetByPath;
        static uint32_t s_FirstFreeSlot;
};


// Asset Definitions
template <class T> requires std::derived_from<T, IAsset>
Asset<T>::Asset() : m_Id(UINT32_MAX) {}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::Asset(uint32_t id) : m_Id(id)
{
        if (IsValid())
                AssetManager::Increment(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::Asset(const Asset<T>& asset)
{
        m_Id = asset.m_Id;
        if (IsValid())
                AssetManager::Increment(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::Asset(const Asset<T>&& asset)
{
        m_Id = asset.m_Id;
        if (IsValid())
                AssetManager::Increment(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::~Asset()
{
        if (IsValid())
                AssetManager::Decrement(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
T&
Asset<T>::Convert() const
{
        return AssetManager::GetData<T>(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
T*
Asset<T>::operator ->() const
{
        return &Convert();
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::operator T&() const
{
        return AssetManager::GetData<T>(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>::operator uint32_t() const
{
        return m_Id;
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>&
Asset<T>::operator=(const Asset<T>& asset)
{
        if (IsValid())
                AssetManager::Decrement(m_Id);

        m_Id = asset.m_Id;
        if (IsValid())
                AssetManager::Increment(m_Id);

        return *this;
}

template <class T> requires std::derived_from<T, IAsset>
Asset<T>&
Asset<T>::operator=(Asset<T>&& asset)
{
        if (IsValid())
                AssetManager::Decrement(m_Id);

        m_Id = asset.m_Id;
        asset.m_Id = UINT32_MAX;

        return *this;
}

template <class T> requires std::derived_from<T, IAsset>
uint32_t
Asset<T>::GetId() const
{
        return m_Id;
}

template <class T> requires std::derived_from<T, IAsset>
const std::string&
Asset<T>::GetPath() const
{
        return AssetManager::GetPath(m_Id);
}

template <class T> requires std::derived_from<T, IAsset>
bool
Asset<T>::IsValid() const
{
        return m_Id != UINT32_MAX &&
               T().GetType() == AssetManager::GetAssetType(m_Id);
}


// AssetManager definitions
template <class T> requires std::derived_from<T, IAsset>
Asset<T>
AssetManager::Get(const std::string& path)
{
        if (auto res = s_AssetByPath.find(path); res != s_AssetByPath.end())
                return Asset<T>(res->second);

        std::filesystem::path absolute = Utils::Path::FromRelative(path);
        if (!std::filesystem::exists(absolute))
                ResourceAssetifier::AssetifyFolder(absolute.parent_path());

        if (!std::filesystem::exists(absolute))
                return Asset<T>();

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

        s_AssetByPath[path] = slot;
        s_Assets[slot] = NewUniq<T>();
        s_Assets[slot]->Deserialize(absolute);
        s_AssetTypes[slot] = s_Assets[slot]->GetType();

        BOREK_ENGINE_INFO("|AssetManager| - Created Asset {}", path);
        return Asset<T>(slot);
}

template <class T> requires std::derived_from<T, IAsset>
T&
AssetManager::GetData(uint32_t id)
{
        return *RCAST<T*>(s_Assets[id].get());
}


}  // namespace Borek

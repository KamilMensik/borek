// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

#include "Include/Base/Entity.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SceneAsset.h"

namespace Borek {

struct PrefabComponent {
        std::filesystem::file_time_type last_modified;
        Asset<SceneAsset> scene;

        PrefabComponent();

        void
        Update(Entity node);
};

}  // namespace Borek

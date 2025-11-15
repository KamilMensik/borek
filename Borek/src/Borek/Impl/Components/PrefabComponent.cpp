// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>
#include "Include/Base/Application.h"

#include "Include/Components/PrefabComponent.h"
#include "Include/Engine/SceneSerializer.h"

namespace Borek {

namespace fs = std::filesystem;

PrefabComponent::PrefabComponent()
{
        last_modified = fs::file_time_type::min();
}

void
PrefabComponent::Update(Entity node)
{
        if (!scene.IsValid()) return;
        if (last_modified >= fs::last_write_time(scene->scene_path)) return;
        last_modified = fs::last_write_time(scene->scene_path);

        SceneSerializer(Application::GetScene())
                .Deserialize(scene->scene_path, node);
}

}  // namespace Borek

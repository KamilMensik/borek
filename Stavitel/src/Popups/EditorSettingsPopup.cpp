// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstring>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Include/Base/Node.h>
#include <Include/Debug/Log.h>
#include <Include/Base/Application.h>
#include <Include/Core.h>

#include "EditorSettingsPopup.h"
#include "EditorSettings.h"

namespace Borek {
namespace Popups {

bool
EditorSettingsPopup::Tick()
{
        bool is_open = true;
        ImGui::Begin("Editor Settings", &is_open);

        ImGui::Checkbox("Show Collision Shapes",
                        &EditorSettings::show_collision_shapes);

        if (ImGui::Button("End")) {
                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek


// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Popup.h"
#include "imgui.h"

namespace Borek {

bool
Popup::Tick()
{
        if (m_Initialized)
                return false;

        m_Initialized = true;

        ImVec2 pos = ImGui::GetMousePos();
        ImGui::SetNextWindowPos({pos.x - 20, pos.y - 20});
        return false;
}

}  // namespace Borek

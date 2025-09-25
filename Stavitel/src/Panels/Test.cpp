// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <imgui.h>
#include <Include/Objects/Font.h>

#include "Test.h"

namespace Borek {
namespace Panels {

Test::Test()
{
        //m_Font = Font(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"));
}

void
Test::OnImguiRender()
{
        return;
        ImGui::Begin("Test");
        ImGui::Image(m_Font.GetTexture()->GetId(), ImVec2(171, 171), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
}

}  // namespace Panels
}  // namespace Borek

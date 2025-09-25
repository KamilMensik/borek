// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "imgui_internal.h"
#include <imgui.h>

#include "ConsolePanel.h"

namespace Borek {
namespace Panels {

static int scroll_to = -1;

void ConsolePanel::OnImguiRender()
{
        if (ImGui::Begin("Console")) {
                if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
                {
                        for (int i = 0; auto& msg : m_Messages) {
                                const glm::vec4& col = msg.second;
                                ImVec4 color = { col.r, col.g, col.b, col.a };
                                ImGui::PushStyleColor(ImGuiCol_Text, color);
                                ImGui::Text("%s", msg.first.c_str());
                                ImGui::PopStyleColor();

                                if (scroll_to == i) {
                                        ImGui::ScrollToItem();
                                        scroll_to = -1;
                                }

                                i++;
                        }
                        ImGui::EndListBox();
                }

                if (ImGui::Button("Reset"))
                        Reset();
        }

        ImGui::End();
}

void
ConsolePanel::Add(const std::string& str, const glm::vec4& color)
{
        scroll_to = m_Messages.size();
        m_Messages.emplace_back(str, color);
}

void
ConsolePanel::Reset()
{
        m_Messages.clear();
}

}  // namespace Panels
}  // namespace Borek

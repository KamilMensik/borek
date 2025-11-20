// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <filesystem>

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "Misc/FileExplorer/FileExplorerPopup.h"

namespace Borek {
namespace Popups {

namespace fs = std::filesystem;

constexpr ImGuiWindowFlags base_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings;

FileExplorerPopup::FileExplorerPopup(const fs::path& context_path, State s)
        : m_ContextPath(context_path), m_State(s)
{
}

bool
FileExplorerPopup::Tick()
{
        bool is_open = true;

        if (!m_Initialized) {
                ImVec2 pos = ImGui::GetMousePos();
                ImGui::SetNextWindowPos({pos.x - 20, pos.y - 20});
                m_Initialized = true;
        }

        if (!ImGui::Begin("##FileExplorerPopup", &is_open, base_flags)) {
                ImGui::End();
                return false;
        }
        switch (m_State) {
        case State::kNormal:
                if (ImGui::Button("Create Directory")) {
                        m_State = State::kCreateDir;
                        m_Input = "";
                }
                break;
        case State::kCreateDir:
                if (ImGui::Button("<"))
                        m_State = State::kNormal;
                ImGui::SameLine();
                ImGui::InputText("Name", &m_Input);
                if (ImGui::Button("Cancel"))
                        is_open = false;

                ImGui::SameLine();
                if (ImGui::Button("Create")) {
                        fs::create_directory(m_ContextPath / m_Input);
                        is_open = false;
                }
                break;
        case State::kFileContextMenu:
                if (ImGui::Button("Rename File")) {
                        m_State = State::kFileRename;
                        m_Input = "";
                }
                if (ImGui::Button("Delete File")) {
                        fs::remove(m_ContextPath);
                        is_open = false;
                }
                break;
        case State::kFileRename:
                if (ImGui::Button("<"))
                        m_State = State::kFileContextMenu;
                ImGui::SameLine();
                ImGui::InputText("New name", &m_Input);
                if (ImGui::Button("Cancel"))
                        is_open = false;
                if (ImGui::Button("Rename")) {
                        fs::rename(m_ContextPath, m_ContextPath.parent_path() / m_Input);
                        is_open = false;
                }
        }

        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                is_open = false;
        }

        ImGui::End();

        return !is_open;
}

}  // namespace Popups
}  // namespace Borek

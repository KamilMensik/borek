// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Popups/TilemapAssetFormPopup.h"
#include <filesystem>

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "Popups/AssetPanelPopup.h"
#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

namespace fs = std::filesystem;

constexpr ImGuiWindowFlags base_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings;

AssetPanelPopup::AssetPanelPopup(const fs::path& context_path, State s)
        : m_ContextPath(context_path), m_State(s)
{
}

bool
AssetPanelPopup::Tick()
{
        Popup::Tick();
        bool is_open = true;
        if (!ImGui::Begin("##AssetPanelPopup", &is_open, base_flags)) {
                ImGui::End();
                return false;
        }
        switch (m_State) {
        case State::kNormal:
                if (ImGui::Button("Create Directory")) {
                        m_State = State::kCreateDir;
                        m_Input = "";
                }
                if (ImGui::Button("New Scene")) {
                        m_State = State::kCreateScene;
                        m_Input = "";
                }
                if (ImGui::Button("Create Tilemap")) {
                        Application::OpenPopup(new TilemapAssetFormPopup(m_ContextPath));
                        is_open = false;
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
        case State::kCreateScene:
                if (ImGui::Button("<"))
                        m_State = State::kNormal;
                ImGui::SameLine();
                ImGui::InputText("New name", &m_Input);
                if (ImGui::Button("Cancel"))
                        is_open = false;
                if (ImGui::Button("Create")) {
                        SceneAsset scn;
                        fs::path path = (m_ContextPath / m_Input);
                        scn.scene_path = path.replace_extension("scnf");
                        scn.Serialize(path.replace_extension("scn"));
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
                break;
        }

        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                is_open = false;
        }

        ImGui::End();

        return !is_open;
}

}  // namespace Popups
}  // namespace Borek

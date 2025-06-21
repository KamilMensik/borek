// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include "Include/Objects/Sprite.h"
#include <cstring>
#include <filesystem>

#include <imgui.h>

#include <Include/Engine/Utils/Settings.h>

#include "./AssetsPanel.h"

namespace Borek {
namespace Panels {

static Ref<Graphics::Texture2D> folder_icon;
static Ref<Graphics::Texture2D> basic_file_icon;
static Ref<Graphics::Texture2D> important_folder_icon;

Assets::Assets() {
        m_CurrentPath = Utils::Settings::Instance().current_project_path;
        folder_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FolderBasic.png"));
        basic_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileBasic.png"));
        important_folder_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FolderImportant.png"));
};

void Assets::FileButton(const std::filesystem::directory_entry& file, float size) {
        if (file.is_directory()) {
                if (ImGui::ImageButton(file.path().c_str(), folder_icon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0))) {
                        m_CurrentPath = file.path();
                }
        } else {
                ImGui::ImageButton(file.path().c_str(), basic_file_icon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
                if (ImGui::BeginDragDropSource()) {
                        auto relative_path = std::filesystem::relative(file.path(), Utils::Settings::Instance().current_project_path);
                        ImGui::SetDragDropPayload("ASSET_ITEM",
                                                  relative_path.c_str(),
                                                  relative_path.string().length());
                        ImGui::EndDragDropSource();
                }
        }
        ImGui::TextWrapped("%s", file.path().filename().c_str());
}

void Assets::OnImguiRender()
{
        ImGui::Begin("Assets");

        float size = 120;
        float padding = 8;

        float avail = ImGui::GetContentRegionAvail().x;
        unsigned columns = avail / (size + padding);
        if (columns == 0) columns = 1;

        ImGui::Columns(columns, nullptr, false);

        if (m_CurrentPath != Utils::Settings::Instance().current_project_path) {
                if (ImGui::ImageButton("../", important_folder_icon->GetId(),
                                       ImVec2(size, size), ImVec2(0, 1),
                                       ImVec2(1, 0))) {
                        m_CurrentPath = m_CurrentPath.parent_path();
                }
                ImGui::TextWrapped("../");
                ImGui::NextColumn();
        }

        for (auto& file : std::filesystem::directory_iterator(m_CurrentPath)) {
                FileButton(file, size);
                ImGui::NextColumn();
        }

        ImGui::Columns(1);

        ImGui::End();
}

}  // namespace Panels
}  // namespace Borek

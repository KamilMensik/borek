// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Graphics/Texture.h"
#include "Include/Objects/Sprite.h"
#include <cstring>
#include <filesystem>

#include <imgui.h>

#include <Include/Engine/Utils/Settings.h>
#include <istream>

#include "./AssetsPanel.h"

namespace Borek {
namespace Panels {

static Ref<Graphics::Texture2D> folder_icon;
static Ref<Graphics::Texture2D> important_folder_icon;
static Ref<Graphics::Texture2D> basic_file_icon;
static Ref<Graphics::Texture2D> config_file_icon;
static Ref<Graphics::Texture2D> image_file_icon;
static Ref<Graphics::Texture2D> key_file_icon;
static Ref<Graphics::Texture2D> object_file_icon;
static Ref<Graphics::Texture2D> script_file_icon;
static Ref<Graphics::Texture2D> sound_file_icon;
static Ref<Graphics::Texture2D> text_file_icon;

Assets::Assets() {
        m_CurrentPath = Utils::Settings::Instance().current_project_path;
        folder_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FolderBasic.png"));
        basic_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileBasic.png"));
        important_folder_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FolderImportant.png"));
        config_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileConfig.png"));
        image_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileImage.png"));
        key_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileKey.png"));
        object_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileObject.png"));
        script_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileScript.png"));
        sound_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileSound.png"));
        text_file_icon = Graphics::Texture2D::Create(ASSET_PATH("assets/EditorIcons/FileText.png"));
};

bool Assets::FileButton(const std::filesystem::directory_entry& file, float size) {
        if (file.is_directory()) {
                if (ImGui::ImageButton(file.path().c_str(), folder_icon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0))) {
                        m_CurrentPath = file.path();
                }
                ImGui::TextWrapped("%s", file.path().filename().c_str());
                return true;
        } else {
                if (file.path().extension() != ".brass")
                        return false;

                std::ifstream asset(file.path());
                uint32_t asset_type;
                std::filesystem::path real_path;

                asset >> asset_type >> real_path;

                uint32_t button_tex_id;
                switch (asset_type) {
                case Asset::Type::kImage:
                        button_tex_id = image_file_icon->GetId();   
                        break;
                case Asset::Type::kScript:
                        button_tex_id = script_file_icon->GetId();
                        break;
                }

                ImGui::ImageButton(real_path.c_str(), button_tex_id, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
                if (ImGui::BeginDragDropSource()) {
                        auto relative_path = std::filesystem::relative(real_path, Utils::Settings::Instance().current_project_path);
                        ImGui::SetDragDropPayload("ASSET_ITEM",
                                                  relative_path.c_str(),
                                                  relative_path.string().length());
                        ImGui::EndDragDropSource();
                }

                ImGui::TextWrapped("%s", real_path.filename().c_str());
                return true;
        }
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
                if (FileButton(file, size))
                        ImGui::NextColumn();
        }

        ImGui::Columns(1);

        ImGui::End();
}

}  // namespace Panels
}  // namespace Borek

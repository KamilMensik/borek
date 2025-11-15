// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstring>
#include <filesystem>

#include <imgui.h>

#include <Include/Base/Application.h>
#include <Include/Engine/Utils/StringHelpers.h>
#include <Include/Engine/Utils/Settings.h>
#include <Include/Core.h>
#include <Include/Engine/Assets/Asset.h>
#include <Include/Graphics/Texture.h>
#include <Include/Engine/Assets/SoundAsset.h>
#include <Include/Engine/Assets/TexAsset.h>

#include "Events/Events.h"
#include "Popups/TilemapAssetFormPopup.h"
#include "./AssetsPanel.h"

namespace Borek {
namespace Panels {

static TexAsset folder_icon;
static TexAsset important_folder_icon;
static TexAsset basic_file_icon;
static TexAsset config_file_icon;
static TexAsset image_file_icon;
static TexAsset key_file_icon;
static TexAsset object_file_icon;
static TexAsset script_file_icon;
static TexAsset sound_file_icon;
static TexAsset text_file_icon;

enum AssetType {
        kImage,
        kScript,
        kSpriteSheet,
        kTileMap,
        kSound,
};

Assets::Assets() {
        m_CurrentPath = Utils::Settings::Instance().current_project_path;
        folder_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FolderBasic.png"));
        basic_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileBasic.png"));
        important_folder_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FolderImportant.png"));
        config_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileConfig.png"));
        image_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileImage.png"));
        key_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileKey.png"));
        object_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileObject.png"));
        script_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileScript.png"));
        sound_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileSound.png"));
        text_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileText.png"));
};

bool Assets::FileButton(const std::filesystem::directory_entry& file, float size) {
        if (file.is_directory()) {
                ImGui::ImageButton(file.path().c_str(), folder_icon.texture->GetId(),
                                   ImVec2(size, size), ImVec2(0, 1),
                                   ImVec2(1, 0));
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        m_CurrentPath = file.path();
                }
                ImGui::TextWrapped("%s", file.path().filename().c_str());
                return true;
        } else {
                uint32_t asset_type;

                switch (Hash(file.path().extension())) {
                case Hash(".tex"):
                        asset_type = AssetType::kImage;
                        break;
                case Hash(".scr"):
                        asset_type = AssetType::kScript;
                        break;
                case Hash(".sst"):
                        asset_type = AssetType::kSpriteSheet;
                        break;
                case Hash(".tmap"):
                        asset_type = AssetType::kTileMap;
                        break;
                case Hash(".snd"):
                        asset_type = AssetType::kSound;
                        break;
                default:
                        return false;
                }

                uint32_t button_tex_id;
                switch (asset_type) {
                case AssetType::kImage:
                        button_tex_id = image_file_icon.texture->GetId();   
                        break;
                case AssetType::kScript:
                        button_tex_id = script_file_icon.texture->GetId();
                        break;
                case AssetType::kSpriteSheet:
                        button_tex_id = image_file_icon.texture->GetId();
                        break;
                case AssetType::kTileMap:
                        button_tex_id = key_file_icon.texture->GetId();
                        break;
                case AssetType::kSound:
                        button_tex_id = sound_file_icon.texture->GetId();
                        break;
                }

                if (ImGui::ImageButton(file.path().c_str(), button_tex_id,
                                       { size, size }, { 0, 1 }, { 1, 0 })) {
                        Application::SendEvent(new AssetPanelSelectedEvent(file.path()));
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        switch (asset_type) {
                        case AssetType::kTileMap: {
                                Application::OpenPopup(new Popups::TilemapAssetFormPopup(file.path(), file.path()));
                                break;
                        }
                        default:
                                break;
                        }
                }

                if (ImGui::BeginDragDropSource()) {
                        auto relative_path = std::filesystem::relative(file.path(), Utils::Settings::Instance().current_project_path);
                        ImGui::SetDragDropPayload("ASSET_ITEM",
                                                  relative_path.c_str(),
                                                  relative_path.string().length());
                        ImGui::EndDragDropSource();
                }

                ImGui::TextWrapped("%s", file.path().filename().c_str());
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

        if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                if (ImGui::MenuItem("Create Tilemap")) {
                        Application::OpenPopup(new Popups::TilemapAssetFormPopup(m_CurrentPath));
                }

                ImGui::EndPopup();
        }

        ImGui::Columns(columns, nullptr, false);

        if (m_CurrentPath != Utils::Settings::Instance().current_project_path) {
                ImGui::ImageButton("../", important_folder_icon.texture->GetId(),
                                       ImVec2(size, size), ImVec2(0, 1),
                                       ImVec2(1, 0));
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
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

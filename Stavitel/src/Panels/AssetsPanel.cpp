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
#include "Misc/FileExplorer/FileExplorer.h"
#include "./AssetsPanel.h"

namespace Borek {
namespace Panels {

namespace fs = std::filesystem;

enum AssetType {
        kImage,
        kScript,
        kSpriteSheet,
        kTileMap,
        kSound,
};

static void
on_asset_doubleclicked(const fs::path& path)
{
        switch (Hash(path.extension())) {
        case Hash(".tmap"): {
                Application::OpenPopup(new Popups::TilemapAssetFormPopup(path, path));
                break;
        }
        default:
                break;
        }
}


Assets::Assets() {
        m_CurrentPath = Utils::Settings::Instance().current_project_path;

        FileExplorerCallbacks fe_callbacks = FileExplorer::GetBaseCallbacks();
        fe_callbacks.file_selected = [](const fs::path& path) {
                Application::SendEvent<AssetPanelSelectedEvent>(path);
        };
        fe_callbacks.file_double_clicked = on_asset_doubleclicked;

        FileExplorer::OpenEX("Assets", FileExplorerType_Basic, m_CurrentPath,
                             FileExplorer::GetBaseAvailExtensions(),
                             FileExplorerFlags_None, fe_callbacks);
};

void Assets::OnImguiRender()
{
        FileExplorer::Begin("Assets");
        FileExplorer::End();

        //if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        //        if (ImGui::MenuItem("Create Tilemap")) {
        //                Application::OpenPopup(new Popups::TilemapAssetFormPopup(m_CurrentPath));
        //        }

        //        ImGui::EndPopup();
        //}
}

}  // namespace Panels
}  // namespace Borek

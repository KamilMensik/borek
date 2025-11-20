// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/ext/vector_float4.hpp>

#include <Borek/Include/Core.h>
#include <Borek/Include/Debug/Log.h>
#include <Borek/Include/Engine/Utils/FileUtils.h>
#include <Borek/Include/Engine/Utils/Settings.h>
#include <Borek/Include/Engine/Assets/Asset.h>
#include <Borek/Include/Engine/Assets/SpriteSheetAsset.h>
#include <Borek/Include/Engine/Assets/TilemapAsset.h>
#include <Borek/Include/Engine/Utils/PathHelpers.h>

#include "Popups/TilemapAssetFormPopup.h"
#include "Misc/FileExplorer/FileExplorer.h"

namespace Borek {
namespace Popups {

namespace fs = std::filesystem;

TilemapAssetFormPopup::TilemapAssetFormPopup(const fs::path& path)
        : m_Path(path), m_NewSpriteSheet(true)
{
        m_Tilemap = NewUniq<TilemapAsset>();
}

TilemapAssetFormPopup::TilemapAssetFormPopup(const fs::path& path,
                                             const fs::path& tmap_path)
        : m_Path(path), m_NewSpriteSheet(false)
{
        m_Tilemap = NewUniq<TilemapAsset>();
        m_Tilemap->LoadFrom(tmap_path);
        m_SpriteSheet = m_Tilemap->sprite_sheet;
        m_SpriteSheetPath = m_SpriteSheet.GetPath();
        m_AssetName = m_Path.filename();
}

bool TilemapAssetFormPopup::Tick()
{
        bool is_open = true;
        ImGui::Begin("Tilemap Form", &is_open);

        ImGui::InputText("Name", &m_AssetName);

        if (ImGui::InputText("Asset Path", &m_SpriteSheetPath)) {
                if (fs::exists(Utils::Path::FromRelative(m_SpriteSheetPath)) && fs::path(m_SpriteSheetPath).extension() == ".sst") {
                        m_SpriteSheet = AssetManager::Get<SpriteSheetAsset>(m_SpriteSheetPath);
                        m_Tilemap->tile_coliders.resize(m_SpriteSheet->GetCells());
                } else {
                        m_SpriteSheet = Asset<SpriteSheetAsset>();
                }
        }

        ImGui::SameLine();
        if (ImGui::Button("Open")) {
                FileExplorer::Open("Open Spritesheet", FileExplorerType_OpenFile,
                                   Utils::Settings::Instance().current_project_path,
                                   { ".sst" });
        }

        if (FileExplorer::Begin("Open Spritesheet")) {
                const std::string& res = FileExplorer::GetSelected();
                if (!res.empty()) {
                        m_SpriteSheetPath = Utils::Path::ToRelative(res);
                }

                if (fs::path(m_SpriteSheetPath).extension() == ".sst") {
                        m_SpriteSheet = AssetManager::Get<SpriteSheetAsset>(m_SpriteSheetPath);
                        m_Tilemap->tile_coliders.resize(m_SpriteSheet->GetCells());
                } else {
                        m_SpriteSheet = Asset<SpriteSheetAsset>();
                }
        }
        FileExplorer::End();

        if (m_SpriteSheet.IsValid()) {
                const uint32_t rows = m_SpriteSheet->GetRows();
                const uint32_t cols = m_SpriteSheet->GetCols();
                if (ImGui::BeginListBox("##ssa", ImVec2(300, 300)))
                {
                        std::vector<std::string> rc(rows * cols);
                        for (int row = rows - 1; row >= 0; row--) {
                                for (int col = 0; col < cols; col++) {
                                        const uint64_t tile_id = col + row * cols;
                                        rc[tile_id] = std::to_string(tile_id) + "+_TAF";
                                        const glm::vec4 bounds = m_SpriteSheet->SubTextureCords(row, col);

                                        if (ImGui::ImageButton(rc[tile_id].c_str(), m_SpriteSheet->texture->GetId(),
                                                     ImVec2(m_SpriteSheet->step_x * 3, m_SpriteSheet->step_y * 3), {bounds.x, bounds.w}, {bounds.z, bounds.y},
                                                               ImVec4(0, 0, 0, 0),
                                                               m_Tilemap->tile_coliders[tile_id].Exists() ? ImVec4(0.5, 1, 0.5, 1) : ImVec4(0.8, 0.8, 0.8, 1))) {
                                                if (!m_Tilemap->tile_coliders[tile_id].Exists()) {
                                                        m_Tilemap->tile_coliders[tile_id] = { 0, 0, SCAST<uint8_t>(m_SpriteSheet->step_x), SCAST<uint8_t>(m_SpriteSheet->step_y) };
                                                } else {
                                                        m_Tilemap->tile_coliders[tile_id].Reset();
                                                }
                                        }

                                        if (col != (cols - 1))
                                                ImGui::SameLine();
                                }
                        }

                        ImGui::EndListBox();
                }
        }

        if (ImGui::Button(m_NewSpriteSheet ? "Create" : "Update")) {
                m_Tilemap->sprite_sheet = m_SpriteSheet;

                if (m_NewSpriteSheet) {
                        m_Tilemap->Serialize(m_Path / m_AssetName);
                        AssetManager::Refresh(Utils::Path::ToRelative(m_Path), std::move(m_Tilemap));
                } else {
                        if (m_AssetName != m_Path.filename())
                                fs::remove(m_Path);

                        m_Path.replace_filename(m_AssetName);
                        m_Tilemap->Serialize(m_Path);
                        AssetManager::Refresh(Utils::Path::ToRelative(m_Path), std::move(m_Tilemap));
                }

                is_open = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek

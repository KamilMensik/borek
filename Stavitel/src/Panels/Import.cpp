// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/Utils/StringHelpers.h"
#include <imgui.h>

#include "Import.h"

namespace Borek {
namespace Panels {

Import::Import()
{
}

void
Import::OnImguiRender()
{
        ImGui::Begin("Import");
        if (!m_SelectedAsset.empty()) {
                switch (Hash(m_SelectedAsset.extension())) {
                case Hash(".scr"):
                        ImGui::Text("Script");
                        break;
                case Hash(".tex"):
                        ImGui::Text("Texture");
                        ImGui::Image(m_AssetTexture->GetId(), {200, 200},
                                     {0, 1}, {1, 0});
                        break;
                }
        }
        ImGui::End();
}

void
Import::SetSelectedAsset(const std::string& asset_path)
{
        m_SelectedAsset = asset_path;
        if (m_SelectedAsset.extension() == ".tex") {
                m_AssetTexture = AssetManager::GetTextureRaw(asset_path);
        }
}

}  // namespace Panels
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/Utils/FileUtils.h"
#include "Include/Engine/Utils/Settings.h"
#include <algorithm>

#include <filesystem>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Include/Core.h>

#include "Popups/SpriteSheetFormPopup.h"

namespace Borek {
namespace Popups {

namespace fs = std::filesystem;

SpriteSheetFormPopup::SpriteSheetFormPopup(const fs::path& path)
        : m_Path(path), m_NewSpriteSheet(true)
{
        m_SpriteSheet = NewRef<SpriteSheet>();
}

SpriteSheetFormPopup::SpriteSheetFormPopup(const fs::path& path,
                                           Ref<SpriteSheet> spritesheet)
        : m_SpriteSheet(spritesheet), m_Path(path), m_NewSpriteSheet(false) {}

bool SpriteSheetFormPopup::Tick()
{
        bool is_open = true;
        ImGui::Begin("SpriteSheet Form", &is_open);

        int step_x = m_SpriteSheet->GetStepX();
        int step_y = m_SpriteSheet->GetStepY();
        int gap = m_SpriteSheet->GetGap();

        if (fs::exists(m_SpriteSheet->GetTexturePath())) {
                m_SpriteSheet->SetTexture(AssetManager::GetTextureRaw(m_SpriteSheet->GetTexturePath()));
        } else {
                m_SpriteSheet->SetTexture(nullptr);
        }

        if (m_SpriteSheet->GetTexture() != nullptr) {
                ImGui::Image(m_SpriteSheet->GetTexture()->GetId(), {200, 200},
                             {0, 1}, {1, 0});
        }

        ImGui::InputText("Name", &m_AssetName);
        ImGui::InputText("Asset Path",
                         &m_SpriteSheet->GetTexturePath());
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
                const std::string res = Borek::Utils::OpenFileDialog(nullptr, Utils::Settings::Instance().current_project_path.c_str());
                if (!res.empty()) {
                        m_SpriteSheet->GetTexturePath() = res;
                }
        }
        ImGui::InputInt("Step X", &step_x);
        ImGui::InputInt("Step Y", &step_y);
        ImGui::InputInt("Gap", &gap);

        m_SpriteSheet->SetStep(std::max(1, step_x), std::max(1, step_y));
        m_SpriteSheet->SetGap(std::max(0, gap));

        if (ImGui::Button(m_NewSpriteSheet ? "Create" : "Update")) {
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

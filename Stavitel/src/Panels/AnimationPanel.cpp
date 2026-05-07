// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <algorithm>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "Include/Base/Application.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/AnimationAsset.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include "Misc/FileExplorer/FileExplorer.h"

#include "Panels/AnimationPanel.h"

namespace Borek {
namespace Panels {

AnimationPanel::AnimationPanel() : m_PrevTime(Application::GetWindow().GetTime())
{
        m_Asset = NewUniq<AnimationAsset>();
}

AnimationPanel::~AnimationPanel()
{
}

void
AnimationPanel::OnImguiRender()
{
        if (!ImGui::Begin("Animation")) {
                ImGui::End();
                return;
        }

        DrawAnimFileInput();

        if (!m_AssetPath.empty()) {
                DrawSpritesheetInput();
                if (m_Asset->sprite_sheet.IsValid()) {
                        if (!m_CurrentAnimation.empty()) {
                                DrawAnimation();
                        } else {
                                DrawSelectAnimation();
                        }
                }
        }

        ImGui::End();
}

void
AnimationPanel::DrawSpritesheetInput()
{
        Asset<SpriteSheetAsset> sst = m_Asset->sprite_sheet;
        ImGui::Text("spritesheet: %s", sst.IsValid() ? sst.GetPath().c_str() : "");
        ImGui::SameLine();
        if (ImGui::Button("Open##spritesheet")) {
                FileExplorer::Open("Open Spritesheet", FileExplorerType_OpenFile,
                                   Application::ProjectPath(), { ".sst" });
        }

        if (FileExplorer::Begin("Open Spritesheet")) {
                m_Asset->sprite_sheet = AssetManager::Get<SpriteSheetAsset>(
                        Utils::Path::ToRelative(FileExplorer::GetSelected()).string());
        }
        FileExplorer::End();
}

void
AnimationPanel::DrawAnimFileInput()
{
        ImGui::Text("%s", m_AssetPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Open##anim")) {
                FileExplorer::Open("Open Animation", FileExplorerType_OpenFile,
                                   Application::ProjectPath(), { ".anim" });
        }
        ImGui::SameLine();
        if (ImGui::Button("New##anim")) {
                FileExplorer::Open("New Animation", FileExplorerType_SaveFile,
                                   Application::ProjectPath(), { ".anim" });
        }
        ImGui::SameLine();
        if (!m_AssetPath.empty() && ImGui::Button("Save##anim")) {
                std::string path = Utils::Path::FromRelative(m_AssetPath);
                m_Asset->Serialize(path);
                AssetManager::Refresh(m_AssetPath, std::move(m_Asset));
                m_Asset = NewUniq<AnimationAsset>();
                m_Asset->LoadFrom(path);
        }

        if (FileExplorer::Begin("Open Animation")) {
                m_AssetPath = Utils::Path::ToRelative(FileExplorer::GetSelected());
                m_Asset->LoadFrom(FileExplorer::GetSelected());
                m_CurrentAnimationName = "";
        }
        FileExplorer::End();

        if (FileExplorer::Begin("New Animation")) {
                m_AssetPath = Utils::Path::ToRelative(FileExplorer::GetSelected());
                m_Asset = NewUniq<AnimationAsset>();
                m_CurrentAnimationName = "";
        }
        FileExplorer::End();
}

void
AnimationPanel::DrawAnimation()
{
        const SpriteSheetAsset& spritesheet = m_Asset->sprite_sheet.Convert();
        const uint32_t rows = spritesheet.GetRows();
        const uint32_t cols = spritesheet.GetCols();

        if (!m_IsPlayingAnim && ImGui::Button("Play##anim_play")) {
                m_IsPlayingAnim = true;
                m_CurrentAnimationFrame = 1;
                m_PrevTime = Application::GetWindow().GetTime();
        }
        if (m_IsPlayingAnim && ImGui::Button("Stop##anim_stop")) {
                m_IsPlayingAnim = false;
        }

        ImGui::SameLine();
        int fps = m_CurrentAnimation[0];
        ImGui::InputInt("FPS:", &fps);
        m_CurrentAnimation[0] = std::clamp(fps, 0, 144);

        ImGui::SameLine();
        if (ImGui::Button("Done##anim_done")) {
                SaveAnim();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel##anim_cancel")) {
                m_CurrentAnimation = {};
                m_CurrentAnimationName = "";
        }

        ImVec2 avail = ImGui::GetContentRegionAvail();

        const glm::vec4 bounds = spritesheet.SubTextureCords(
                m_CurrentAnimation[m_CurrentAnimationFrame]);
        ImGui::Image(spritesheet.texture->GetId(), { avail.y, avail.y },
                     { bounds.x, bounds.w }, { bounds.z, bounds.y });
        ImGui::SameLine();
        avail.x -= avail.y;

        if (ImGui::BeginListBox("##anim_timeframe", ImVec2(avail.x * (2 / 3.0f),
                                                           avail.y))) {
                const float lx = ImGui::GetCursorPosX();
                const int row_size = (avail.x * (2 / 3.0f)) / 60;

                for (int i = 1; i < m_CurrentAnimation.size(); i++) {
                        uint32_t val = m_CurrentAnimation[i];
                        if (val == UINT32_MAX) break;

                        const glm::vec4 bounds = spritesheet.SubTextureCords(val);
                        const ImVec2 old_pos = ImGui::GetCursorPos();
                        ImGui::Image(spritesheet.texture->GetId(),
                                     ImVec2(50, 50), {bounds.x, bounds.w},
                                     {bounds.z, bounds.y});

                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
                                if (i != 0) {
                                        ImGui::SetCursorPos({ old_pos.x,
                                                              old_pos.y + 25 });
                                        if (ImGui::Button("<")) {
                                                m_CurrentAnimation[i] =
                                                        m_CurrentAnimation[i - 1];
                                                m_CurrentAnimation[i - 1] = val;
                                        }
                                }
                                if (i != m_CurrentAnimation.size() - 2) {
                                        ImGui::SetCursorPos({ old_pos.x + 20,
                                                              old_pos.y + 25 });
                                        if (ImGui::Button(">")) {
                                                m_CurrentAnimation[i] =
                                                        m_CurrentAnimation[i + 1];
                                                m_CurrentAnimation[i + 1] = val;
                                        }
                                }
                                ImGui::SetCursorPos({ old_pos.x + 35,
                                                      old_pos.y });
                                if (ImGui::Button("x")) {
                                        m_CurrentAnimation.erase(
                                                m_CurrentAnimation.begin() + i
                                        );
                                }
                        }

                        if (i % row_size != row_size - 1) {
                                ImGui::SetCursorPos({ old_pos.x + 60,
                                                      old_pos.y });
                        } else {
                                ImGui::SetCursorPos({ lx, old_pos.y + 60 });
                        }
                }

                ImGui::EndListBox();
        }

        ImGui::SameLine();
        if (ImGui::BeginChild("##ssa", ImVec2(avail.x * (1 / 3.0f), avail.y), ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar)) {
                for (int row = rows - 1; row >= 0; row--) {
                        for (int col = 0; col < cols; col++) {
                                const uint64_t tile_id = col + row * cols;
                                ImGui::PushID(tile_id);
                                const glm::vec4 bounds = spritesheet.SubTextureCords(row, col);

                                if (ImGui::ImageButton("sprbtn", spritesheet.texture->GetId(),
                                                ImVec2(spritesheet.step_x * 3,
                                                       spritesheet.step_y * 3),
                                                {bounds.x, bounds.w},
                                                {bounds.z, bounds.y},
                                                ImVec4(0, 0, 0, 0))) {
                                        m_CurrentAnimation.back() = tile_id;
                                        m_CurrentAnimation.emplace_back(UINT32_MAX);
                                }

                                ImGui::PopID();

                                if (col != (cols - 1))
                                        ImGui::SameLine();
                        }
                }

                ImGui::EndChild();
        }

        if (m_IsPlayingAnim) {
                Time t = Application::GetWindow().GetTime();
                if (t.Seconds() - m_PrevTime.Seconds() >= 1.0f / m_CurrentAnimation[0]) {
                        m_PrevTime = t;
                        if (m_CurrentAnimation[++m_CurrentAnimationFrame] == UINT32_MAX) {
                                m_CurrentAnimationFrame = 1;
                        }
                }
        }
}

void
AnimationPanel::DrawSelectAnimation()
{
        ImGui::InputText("", &m_CurrentAnimationName);
        ImGui::SameLine();
        if (ImGui::Button("+")) {
                m_CurrentAnimation = { 24, UINT32_MAX };
        }

        for (auto it : m_Asset->animation_by_name) {
                ImGui::PushID(it.first.Str().c_str());
                ImGui::Text("%s", it.first.Str().c_str());
                ImGui::SameLine();
                if (ImGui::Button(">")) {
                        m_CurrentAnimationName = it.first.Str();
                        m_CurrentAnimation = {};

                        uint32_t* val = &m_Asset->animation_frames[it.second];
                        while (*val != UINT32_MAX) {
                                m_CurrentAnimation.emplace_back(*val);
                                val++;
                        }
                        m_CurrentAnimation.emplace_back(UINT32_MAX);
                }
                ImGui::SameLine();
                if (ImGui::Button("X")) {
                        DeleteAnim(it.first);
                }
                ImGui::PopID();
        }
}


void
AnimationPanel::DeleteAnim(const std::string& name)
{
        m_CurrentAnimation = {};
        m_CurrentAnimationName = name;
        SaveAnim();
}

void
AnimationPanel::SaveAnim()
{
        std::vector<uint32_t> new_vec;
        for (auto it : m_Asset->animation_by_name) {
                if (it.first.Str() == m_CurrentAnimationName)
                        continue;

                uint32_t index = new_vec.size();
                uint32_t* val = &m_Asset->animation_frames[it.second];
                while (*val != UINT32_MAX) {
                        new_vec.emplace_back(*val);
                        val++;
                }
                new_vec.emplace_back(UINT32_MAX);
                m_Asset->animation_by_name[it.first] = index;
        }

        if (!m_CurrentAnimation.empty()) {
                m_Asset->animation_by_name[m_CurrentAnimationName] = new_vec.size();
                uint32_t* val = m_CurrentAnimation.data();
                while (*val != UINT32_MAX) {
                        new_vec.emplace_back(*val);
                        val++;
                }
                new_vec.emplace_back(UINT32_MAX);
                m_CurrentAnimation = {};
        } else {
                m_Asset->animation_by_name.erase(m_CurrentAnimationName);
        }

        m_Asset->animation_frames = std::move(new_vec);
}

}  // namespace Panels
}  // namespace Borek

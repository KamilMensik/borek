// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/AssetFlags.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/TexAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include "Misc/SearchBar.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <imgui.h>

#include "Import.h"
#include "Include/Engine/Utils/StringHelpers.h"

namespace fs = std::filesystem;

static const std::array<const char*, 2> filtering_types = {
        "nearest", "linear",
};

namespace Borek {
namespace Panels {

Import::Import()
{
        m_EvHandles[0] = AssetPanelSelectedEvent::AddListener(EVENT_FN(OnAssetPanelSelected));
}

Import::~Import()
{
        AssetPanelSelectedEvent::RemoveListener(m_EvHandles[0]);
}

bool
Import::OnAssetPanelSelected(AssetPanelSelectedEvent& e)
{
        SetSelectedAsset(e.GetAssetPath());
        return false;
}

static bool
TextureImportSettings(TexAsset& tex)
{
        bool modified = false;

        const char* res = filtering_types[tex.flags & TexFlags_Filtering];
        SearchBar(filtering_types, &res, "filtering##filtering");

        uint32_t filtering = 0;
        switch (Hash(res)) {
        case Hash("linear"):
                filtering = TexFlags_Linear;
                break;
        case Hash("nearest"):
                filtering = TexFlags_Nearest;
                break;
        }

        if (filtering != (tex.flags & TexFlags_Filtering)) {
                tex.flags = (~TexFlags_Filtering & tex.flags) | filtering;
                modified = true;
        }

        return modified;
}

static bool
SoundImportSettings(SoundAsset& snd)
{
        bool modified = false;

        BitFlags flags = snd.flags;

        bool looping = flags.HasFlags(SoundFlags_Looping);
        if (ImGui::Checkbox("Looping", &looping)) {
                flags.SetFlags(SoundFlags_Looping, looping);
        }

        bool stream = flags.HasFlags(SoundFlags_Stream);
        if (ImGui::Checkbox("Stream", &stream)) {
                flags.SetFlags(SoundFlags_Stream, stream);
        }

        if (ImGui::SliderFloat("Volume", &snd.volume, 0.0f, 1.0f)) {
                modified = true;
        }

        if (snd.flags != flags) {
                snd.flags = flags;
                modified = true;
        }

        return modified;
}

static bool
SpriteSheetImportSettings(SpriteSheetAsset& ssa)
{

        bool modified = false;
        int step_x = ssa.step_x;
        int step_y = ssa.step_y;
        int gap_x = ssa.gap_x;
        int gap_y = ssa.gap_y;

        const char* res = filtering_types[ssa.flags & TexFlags_Filtering];
        SearchBar(filtering_types, &res, "filtering##filtering");

        ImGui::InputInt("Step X", &step_x);
        ImGui::InputInt("Step Y", &step_y);
        ImGui::InputInt("Gap X", &gap_x);
        ImGui::InputInt("Gap Y", &gap_y);

        step_x = std::clamp(step_x, 1, ssa.width);
        step_y = std::clamp(step_y, 1, ssa.height);
        gap_x = std::clamp(gap_x, 0, ssa.width);
        gap_y = std::clamp(gap_y, 0, ssa.height);

        if (step_x != ssa.step_x) {
                modified = true;
                ssa.step_x = step_x;
        }
        if (step_y != ssa.step_y) {
                modified = true;
                ssa.step_y = step_y;
        }
        if (gap_x != ssa.gap_x) {
                modified = true;
                ssa.gap_x = gap_x;
        }
        if (gap_y != ssa.gap_y) {
                modified = true;
                ssa.gap_y = gap_y;
        }

        uint32_t filtering = 0;
        switch (Hash(res)) {
        case Hash("linear"):
                filtering = TexFlags_Linear;
                break;
        case Hash("nearest"):
                filtering = TexFlags_Nearest;
                break;
        }

        if (filtering != (ssa.flags & TexFlags_Filtering)) {
                ssa.flags = (~TexFlags_Filtering & ssa.flags) | filtering;
                modified = true;
        }

        return modified;
}

void
Import::OnImguiRender()
{
        ImGui::Begin("Import");
        if (!m_SelectedAssetPath.empty()) {
                switch (Hash(m_SelectedAssetPath.extension())) {
                case Hash(".scr"):
                        ScriptImport();
                        break;
                case Hash(".tex"):
                        TextureImport();
                        break;
                case Hash(".sst"):
                        SpriteSheetImport();
                        break;
                case Hash(".snd"):
                        SoundImport();
                        break;
                }
        }
        ImGui::End();
}

void
Import::SetSelectedAsset(const std::string& asset_path)
{
        m_SelectedAssetPath = asset_path;

        switch (Hash(m_SelectedAssetPath.extension())) {
        case Hash(".tex"):
                m_SelectedAsset = NewUniq<TexAsset>();
                break;
        case Hash(".sst"):
                m_SelectedAsset = NewUniq<SpriteSheetAsset>();
                break;
        case Hash(".snd"):
                m_SelectedAsset = NewUniq<SoundAsset>();
                break;
        default:
                m_SelectedAsset = nullptr;
                break;
        }

        if (m_SelectedAsset)
                m_SelectedAsset->LoadFrom(m_SelectedAssetPath);
}

void
Import::TextureImport()
{
        TexAsset& tex = RCAST<TexAsset&>(*m_SelectedAsset);
        fs::path relative_path = Utils::Path::ToRelative(m_SelectedAssetPath);

        ImGui::Text("Sprite");
        ImGui::SameLine();
        if (ImGui::Button("Convert to spritesheet")) {
                AssetManager::Remove(relative_path);
                fs::remove(m_SelectedAssetPath);
                m_SelectedAssetPath.replace_extension(".sst");

                auto ssa = NewUniq<SpriteSheetAsset>();
                ssa->width = tex.width;
                ssa->height = tex.height;
                ssa->flags = tex.flags;
                ssa->channels = tex.channels;
                ssa->data = tex.data;
                ssa->texture = tex.texture;
                ssa->Serialize(m_SelectedAssetPath);

                tex.data = nullptr;
                m_SelectedAsset = std::move(ssa);
                return;
        }

        if (TextureImportSettings(tex)) {
                m_SelectedAsset->Serialize(m_SelectedAssetPath);
                AssetManager::Refresh(relative_path, std::move(m_SelectedAsset));
                m_SelectedAsset = NewUniq<TexAsset>();
                m_SelectedAsset->LoadFrom(m_SelectedAssetPath);
        }

        ImGui::Image(tex.texture->GetId(), {300, 300}, {0, 1}, {1, 0});
}

void
Import::SpriteSheetImport()
{
        SpriteSheetAsset& ssa = RCAST<SpriteSheetAsset&>(*m_SelectedAsset);
        fs::path relative_path = Utils::Path::ToRelative(m_SelectedAssetPath);

        ImGui::Text("SpriteSheet");
        ImGui::SameLine();
        if (ImGui::Button("Convert to sprite")) {
                AssetManager::Remove(relative_path);
                fs::remove(m_SelectedAssetPath);
                m_SelectedAssetPath.replace_extension(".tex");

                auto tex = NewUniq<TexAsset>();
                tex->width = ssa.width;
                tex->height = ssa.height;
                tex->flags = ssa.flags;
                tex->channels = ssa.channels;
                tex->data = ssa.data;
                tex->texture = ssa.texture;
                tex->Serialize(m_SelectedAssetPath);

                ssa.data = nullptr;
                m_SelectedAsset = std::move(tex);
                return;
        }

        if (SpriteSheetImportSettings(ssa)) {
                m_SelectedAsset->Serialize(m_SelectedAssetPath);
                AssetManager::Refresh(relative_path, std::move(m_SelectedAsset));
                m_SelectedAsset = NewUniq<SpriteSheetAsset>();
                m_SelectedAsset->LoadFrom(m_SelectedAssetPath);
        }


        const uint32_t rows = ssa.GetRows();
        const uint32_t cols = ssa.GetCols();
        if (ImGui::BeginListBox("##ssa", ImVec2(300, 300)))
        {
                for (uint32_t row = 0; row < rows; row++) {
                        for (uint32_t col = 0; col < cols; col++) {
                                const glm::vec4 bounds = ssa.SubTextureCords(row, col);
                                ImGui::Image(ssa.texture->GetId(),
                                             ImVec2(ssa.step_x * 3, ssa.step_y * 3), {bounds.x, 1 - bounds.y}, {bounds.z, 1 - bounds.w});

                                if (col != (cols - 1))
                                        ImGui::SameLine();
                        }
                }

                ImGui::EndListBox();
        }
}

void

Import::ScriptImport()
{
        ImGui::Text("Script");
}

void
Import::SoundImport()
{
        SoundAsset& snd = RCAST<SoundAsset&>(*m_SelectedAsset);
        fs::path relative_path = Utils::Path::ToRelative(m_SelectedAssetPath);

        ImGui::Text("Sound");
        if (SoundImportSettings(snd)) {
                m_SelectedAsset->Serialize(m_SelectedAssetPath);
                AssetManager::Refresh(relative_path, std::move(m_SelectedAsset));
                m_SelectedAsset = NewUniq<SoundAsset>();
                m_SelectedAsset->Deserialize(m_SelectedAssetPath);
        }
}

}  // namespace Panels
}  // namespace Borek

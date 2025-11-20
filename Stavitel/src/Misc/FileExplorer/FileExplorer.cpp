// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/ResourceEvents.h"
#include "Include/Base/Application.h"
#include "Include/Debug/Log.h"
#include "Misc/FileExplorer/FileExplorerPopup.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>

#include <Borek/Include/Engine/Utils/PathHelpers.h>
#include <Borek/Include/Engine/Assets/TexAsset.h>
#include <Borek/Include/Engine/Utils/Settings.h>
#include <Borek/Include/Engine/Utils/StringHelpers.h>
#include <Borek/Include/Drawing/DrawingGlobals.h>

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "./FileExplorer.h"
#include "Misc/EditorTextures.h"

namespace Borek {

namespace fs = std::filesystem;

static std::pair<fs::path, fs::path> renaming = { "", "" };

static fs::path
normalized_trimed(const fs::path& p)
{
        auto r = p.lexically_normal();
        if (r.has_filename()) return r;
        return r.parent_path();
}

static bool 
is_subpath_of(const fs::path& base, const fs::path& sub)
{
        auto b = normalized_trimed(base);
        auto s = normalized_trimed(sub).parent_path();
        auto m = std::mismatch(b.begin(), b.end(), s.begin(), s.end());

        return m.first == b.end();
}

static void
drag_drop_source(const fs::path& path)
{
        if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("FILE_EXPL_ITEM", path.c_str(),
                                          path.string().length() + 1);
                ImGui::EndDragDropSource();
        }
}

static void
directory_drag_drop_target(const fs::path& dir)
{
        constexpr uint64_t flags = ImGuiDragDropFlags_AcceptPeekOnly;
        if (!ImGui::BeginDragDropTarget())
                return;

        auto payload = ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM", flags);
        if (!payload)
                return;

        const fs::path path = RCAST<const char*>(payload->Data);
        if (is_subpath_of(path, dir))
                return;

        if (ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM"))
                renaming = { path, dir / path.filename() };

        ImGui::EndDragDropTarget();
}

static bool
img_button(const char* str_id, const ImVec2& size, uint32_t tex_id)
{
        return ImGui::ImageButton(str_id, tex_id, size,
                                  ImVec2(0, 1), ImVec2(1, 0));
}

static bool
is_item_doubleclicked()
{
        return ImGui::IsItemHovered() &&
               ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
}

static bool
is_item_rightclicked()
{
        return ImGui::IsItemHovered() &&
               ImGui::IsMouseClicked(ImGuiMouseButton_Right);
}

static bool
is_item_clicked()
{
        return ImGui::IsItemHovered() &&
               ImGui::IsMouseClicked(ImGuiMouseButton_Left);
}

static uint32_t
get_file_tex_id(const fs::path& path, texture_map* t_map = nullptr)
{
        const FileIcons& ficons = EditorTextures::file_icons;

        if (fs::is_directory(path))
                return ficons.folder_icon.texture->GetId();

        switch (Hash(path.extension())) {
        case Hash(".tex"):
                if (!t_map)
                        return ficons.image_file_icon.texture->GetId();

                if (auto it = t_map->find(path); it != t_map->end()) {
                        return it->second->GetId();
                }

                (*t_map)[path] = Graphics::Texture2D::Create(path);
                return (*t_map)[path]->GetId();
        case Hash(".sst"):
                return ficons.image_file_icon.texture->GetId();
        case Hash(".scr"):
                return ficons.script_file_icon.texture->GetId();
        case Hash(".tmap"):
                return ficons.key_file_icon.texture->GetId();
        case Hash(".snd"):
                return ficons.sound_file_icon.texture->GetId();
        case Hash(".scn"):
                return ficons.object_file_icon.texture->GetId();
        default:
                return UINT32_MAX;
        }
}

static void
file_selected(const fs::path& path)
{

}

static void
file_double_clicked(const fs::path& path)
{

}

static void
file_right_clicked(const fs::path& path)
{
        constexpr auto state = Popups::FileExplorerPopup::State::kFileContextMenu;
        auto popup = new Popups::FileExplorerPopup(path, state);

        Application::OpenPopup(popup);
}

static void
detail_right_clicked(const fs::path& current_dir)
{
        constexpr auto state = Popups::FileExplorerPopup::State::kNormal;
        auto popup = new Popups::FileExplorerPopup(current_dir, state);

        Application::OpenPopup(popup);
}

static void
file_moved(const fs::path& from, const fs::path& to)
{
        Application::SendEvent<ResourceRenamedEvent>(from, to);
}

FileExplorer::FileExplorer()
{
}

const FileExplorerCallbacks&
FileExplorer::GetBaseCallbacks()
{
        static const FileExplorerCallbacks callbacks = {
                .file_selected = file_selected,
                .file_double_clicked = file_double_clicked,
                .file_right_clicked = file_right_clicked,
                .file_moved = file_moved,
                .detail_right_clicked = detail_right_clicked,
        };

        return callbacks;
}

const std::initializer_list<std::string>&
FileExplorer::GetBaseAvailExtensions()
{
        static const std::initializer_list<std::string> ext = {
                ".tex", ".scr", ".sst", ".tmap", ".snd", ".scn",
        };

        return ext;
}


void
FileExplorer::Open(const std::string& name, FileExplorerType type,
                   const std::filesystem::path& workspace,
                   const std::initializer_list<std::string>& selected_extensions,
                   FileExplorerFlags flags)
{
        OpenEX(name, type, workspace, selected_extensions, flags,
               GetBaseCallbacks());
}

void
FileExplorer::OpenEX(const std::string& name, FileExplorerType type,
                     const std::filesystem::path& workspace,
                     const std::initializer_list<std::string>& selected_extensions,
                     FileExplorerFlags flags,
                     const FileExplorerCallbacks& callbacks)
{
        FileExplorer& fe = s_CurrentExplorers[name];
        fe.m_Name = name;
        fe.m_SelectedExtensions = selected_extensions;
        fe.m_Workspace = workspace;
        fe.m_Callbacks = callbacks;
        fe.m_CurrentFolder = workspace;

        BitFlags& fef = fe.m_Flags;
        fef = flags;
        fef.SetFlags(type, true);
        fef.SetFlags(FileExplorerFlags_Open, true);
}

bool
FileExplorer::Begin(const std::string& name)
{
        s_ExplorerStack.emplace_back(name);
        auto it = s_CurrentExplorers.find(name);
        if (it == s_CurrentExplorers.end())
                return false;

        FileExplorer& fe = it->second;
        fe.Draw();

        return fe.m_Flags.HasFlags(FileExplorerFlags_Done);
}

void
FileExplorer::End()
{
        const std::string& name = s_ExplorerStack.back();
        auto it = s_CurrentExplorers.find(name);
        if (it == s_CurrentExplorers.end()) {
                s_ExplorerStack.pop_back();
                return;
        }

        FileExplorer& fe = it->second;
        if (!fe.m_Flags.HasFlags(FileExplorerFlags_Open))
                s_CurrentExplorers.erase(name);
        s_ExplorerStack.pop_back();
}

const std::string&
FileExplorer::GetSelected()
{
        constexpr static std::string empty = "";

        const std::string& name = s_ExplorerStack.back();
        auto it = s_CurrentExplorers.find(name);
        if (it == s_CurrentExplorers.end()) {
                s_ExplorerStack.pop_back();
                return empty;
        }

        return it->second.m_Selected;
}


uint32_t
FileExplorer::GetType()
{
        return m_Flags & (uint32_t)FileExplorerFlags_Type;
}

void
FileExplorer::Draw()
{
        if (!m_Flags.HasFlags(FileExplorerFlags_Open))
                return;

        const bool is_basic = GetType() == FileExplorerType_Basic;
        const float offset = is_basic ? 0.0f : 90.0f;

        if (!ImGui::Begin(m_Name.c_str())) {
                ImGui::End();
                return;
        }

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);

        ImGui::BeginTable("file_explorer_table", 2, ImGuiTableFlags_Resizable);

        ImGui::TableNextColumn();
        ImVec2 size = ImGui::GetContentRegionAvail();

        ImGui::PushID("FEXP-FW");
        if (ImGui::BeginListBox("##fxpf", { size.x, size.y - offset }))
        {
                DrawFolderView(m_Workspace, m_Workspace.parent_path());
                ImGui::EndListBox();
        }
        ImGui::PopID();

        ImGui::TableNextColumn();

        ImGui::InputText("Search", &m_Search);
        size = ImGui::GetContentRegionAvail();

        ImGui::PushID("FEXP-DW");
        if (ImGui::BeginListBox("##fxpd", { size.x, size.y - offset }))
        {
                bool item_hovered = false;
                if (m_Search.empty())
                        item_hovered = DrawDetailView();
                else
                        item_hovered = DrawSearchView();

                if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !item_hovered) {
                        m_Callbacks.detail_right_clicked(m_CurrentFolder);
                }
                ImGui::EndListBox();
        }
        ImGui::PopID();

        if (!renaming.first.empty()) {
                fs::rename(renaming.first, renaming.second);
                BOREK_ENGINE_INFO("Renaming: [{}, {}]", renaming.first.string(), renaming.second.string());
                m_Callbacks.file_moved(renaming.first, renaming.second);
                if (renaming.first == m_CurrentFolder) {
                        m_Textures.clear();
                        m_CurrentFolder = renaming.second;
                }
                renaming.first = "";
        }

        ImGui::EndTable();

        if (!is_basic) {
                ImGui::BeginTable("file_explorer_footer", 3);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableNextColumn();
                ImGui::Text("File name: ");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("", &m_Selected);
                ImGui::PopItemWidth();
                ImGui::TableNextColumn();
                if (ImGui::Button("Open File")) {
                        m_Flags.SetFlags(FileExplorerFlags_Open, false);
                        m_Flags.SetFlags(FileExplorerFlags_Done, true);
                }
                ImGui::TableNextColumn();
                ImGui::Text("Files of type: ");
                ImGui::TableNextColumn();
                ImGui::Text("[.tex, .sst]");
                ImGui::TableNextColumn();
                if (ImGui::Button("Cancel")) {
                        m_Flags.SetFlags(FileExplorerFlags_Open, false);
                }
                ImGui::EndTable();
        }

        ImGui::PopFont();
        ImGui::End();
}
void
FileExplorer::DrawFolderView(const fs::path& path, const fs::path& parent_path)
{
        if (!fs::exists(path)) return;
        const bool is_directory = fs::is_directory(path);
        const bool is_regular_file = fs::is_regular_file(path);

        if (!(is_regular_file || is_directory))
                return;

        if (is_regular_file && !m_SelectedExtensions.contains(path.extension()))
                return;

        const ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_SpanFullWidth |
                ImGuiTreeNodeFlags_DefaultOpen |
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_Leaf * is_regular_file;

        std::string rel_path = fs::relative(path, parent_path);
        const bool open = ImGui::TreeNodeEx(rel_path.c_str(), flags, "%s", "");
        drag_drop_source(path);
        if (is_item_clicked()) {
                m_Selected = path;
                m_Callbacks.file_selected(path);
        }
        if (is_item_doubleclicked())
                m_Callbacks.file_double_clicked(path);
        if (is_item_rightclicked())
                m_Callbacks.file_right_clicked(path);

        if (is_directory) {
                if (is_item_doubleclicked()) {
                        m_Textures.clear();
                        m_CurrentFolder = path;
                        m_Search = "";
                }
                directory_drag_drop_target(path);
        }

        ImGui::SameLine();
        ImGui::Image(get_file_tex_id(path), {32, 32}, {0, 1}, {1, 0});
        ImGui::SameLine();
        ImGui::Text("%s", rel_path.c_str());
        if (!open)
                return;

        if (is_directory) {
                for (auto& file : std::filesystem::directory_iterator(path))
                        DrawFolderView(file, path);
        }
        ImGui::TreePop();
}

bool
FileExplorer::DrawDetailView()
{
        const float size = 120;
        const float padding = 8;
        const float avail = ImGui::GetContentRegionAvail().x;
        unsigned columns = std::max(SCAST<unsigned>(avail / (size + padding + 10)), 1u);
        bool item_hovered = false;

        if (columns == 0) columns = 1;
        ImGui::Columns(columns, nullptr, false);

        if (m_CurrentFolder != Utils::Settings::Instance().current_project_path) {
                const FileIcons& ficons = EditorTextures::file_icons;

                img_button("../", { size, size }, ficons.important_folder_icon.texture->GetId());
                if (is_item_doubleclicked()) {
                        m_Textures.clear();
                        m_CurrentFolder = m_CurrentFolder.parent_path();
                        m_Search = "";
                }
                ImGui::TextWrapped("../");
                ImGui::NextColumn();
        }

        for (auto& file : std::filesystem::directory_iterator(m_CurrentFolder)) {
                std::pair<bool, bool> res = FileButton(file, size);
                if (res.first)
                        ImGui::NextColumn();

                item_hovered |= res.second;
        }

        ImGui::Columns(1);

        return item_hovered;
}

bool
FileExplorer::DrawSearchView()
{
        const float size = 120;
        const float padding = 8;
        const float avail = ImGui::GetContentRegionAvail().x;
        unsigned columns = avail / (size + padding + 10);

        if (columns == 0) columns = 1;
        ImGui::Columns(columns, nullptr, false);

        bool item_hovered = DrawSearchViewHelper(size, m_Workspace);

        ImGui::Columns(1);

        return item_hovered;
}

bool
FileExplorer::DrawSearchViewHelper(float size, const std::filesystem::path& path)
{
        bool item_hovered = false;

        for (auto& file : std::filesystem::directory_iterator(path)) {
                if (strcasestr(file.path().filename().c_str(), m_Search.c_str())) {
                        std::pair<bool, bool> res = FileButton(file, size);
                        if (res.first)
                                ImGui::NextColumn();

                        item_hovered |= res.second;
                }

                if (file.is_directory()) {
                        item_hovered |= DrawSearchViewHelper(size, file.path());
                }
        }

        return item_hovered;
}

std::pair<bool, bool>
FileExplorer::FileButton(const fs::directory_entry& file, float size) {
        const fs::path& path = file.path();

        if (file.is_regular_file() && !m_SelectedExtensions.contains(path.extension()))
                return { false, false };

        const bool is_selected = m_Selected == path;
        const uint32_t tex_id = get_file_tex_id(path, &m_Textures);

        if (is_selected)
                ImGui::PushStyleColor(ImGuiCol_Border, {0.3, 1, 0.4, 1});

        if (img_button(path.c_str(), { size, size }, tex_id)) {
                m_Selected = path;
                m_Callbacks.file_selected(path);
        }

        drag_drop_source(path);
        if (is_item_doubleclicked()) {
                if (file.is_directory()) {
                        m_CurrentFolder = path;
                        m_Search = "";
                        m_Textures.clear();
                }

                m_Callbacks.file_double_clicked(path);
        }

        if (is_item_rightclicked())
                m_Callbacks.file_right_clicked(path);
        if (file.is_directory())
                directory_drag_drop_target(file);
        bool is_hovered = ImGui::IsItemHovered();

        if (is_selected)
                ImGui::PopStyleColor();

        ImGui::TextWrapped("%s", path.filename().c_str());

        return { true, is_hovered };
}

std::vector<std::string> FileExplorer::s_ExplorerStack;
std::unordered_map<std::string, FileExplorer> FileExplorer::s_CurrentExplorers;

}  // namespace Borek

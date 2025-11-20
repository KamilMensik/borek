// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>
#include <initializer_list>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include <vector>

namespace Borek {

enum FileExplorerType {
        FileExplorerType_Basic = 1,
        FileExplorerType_OpenFile = 2,
        FileExplorerType_SaveFile = 3,
};

enum FileExplorerFlags {
        FileExplorerFlags_None = 0,
        FileExplorerFlags_Type = 0b11,
        FileExplorerFlags_Done = 1 << 2,
        FileExplorerFlags_Open = 1 << 3,
        FileExplorerFlags_OnlyDirectories = 1 << 4,
        FileExplorerFlags_AllowWorkspaceChange = 1 << 5,
};

using texture_map = std::unordered_map<std::string, Ref<Graphics::Texture2D>>;
using extension_set = std::unordered_set<std::string>;

struct FileExplorerCallbacks {
        void(*file_selected)(const std::filesystem::path& path);
        void(*file_double_clicked)(const std::filesystem::path& path);
        void(*file_right_clicked)(const std::filesystem::path& path);
        void(*file_moved)(const std::filesystem::path& from,
                          const std::filesystem::path& to);
        void(*detail_right_clicked)(const std::filesystem::path& current_dir);
};

class FileExplorer {
public:
        FileExplorer();

        static void
        Open(const std::string& name, FileExplorerType type,
             const std::filesystem::path& workspace,
             const std::initializer_list<std::string>& selected_extensions = GetBaseAvailExtensions(),
             FileExplorerFlags flags = FileExplorerFlags_None);

        static void
        OpenEX(const std::string& name, FileExplorerType type,
               const std::filesystem::path& workspace,
               const std::initializer_list<std::string>& selected_extensions,
               FileExplorerFlags flags, const FileExplorerCallbacks& callbacks);

        static bool
        Begin(const std::string& name);

        static void
        End();

        static const std::string&
        GetSelected();

        static const FileExplorerCallbacks&
        GetBaseCallbacks();

        static const std::initializer_list<std::string>&
        GetBaseAvailExtensions();

private:
        std::filesystem::path m_Workspace;
        std::filesystem::path m_CurrentFolder;
        std::string m_Name;
        std::string m_Search;
        std::string m_Selected;
        extension_set m_SelectedExtensions;
        texture_map m_Textures;
        FileExplorerCallbacks m_Callbacks;
        BitFlags m_Flags;

        uint32_t
        GetType();

        void
        Draw();

        void
        DrawFolderView(const std::filesystem::path& path,
                       const std::filesystem::path& parent_path = "");

        bool
        DrawDetailView();

        bool
        DrawSearchView();

        bool
        DrawSearchViewHelper(float size, const std::filesystem::path& path);

        std::pair<bool, bool>
        FileButton(const std::filesystem::directory_entry& file, float size);

        static std::vector<std::string> s_ExplorerStack;
        static std::unordered_map<std::string, FileExplorer> s_CurrentExplorers;
};

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/TexAsset.h"

namespace Borek {

struct NodeIcons {
        TexAsset node;
        TexAsset area;
        TexAsset camera;
        TexAsset dynamic_body;
        TexAsset static_body;
        TexAsset sprite;
        TexAsset text;
        TexAsset tilemap;
        TexAsset sound;
};

struct FileIcons {
        TexAsset folder_icon;
        TexAsset important_folder_icon;
        TexAsset basic_file_icon;
        TexAsset config_file_icon;
        TexAsset image_file_icon;
        TexAsset key_file_icon;
        TexAsset object_file_icon;
        TexAsset script_file_icon;
        TexAsset sound_file_icon;
        TexAsset text_file_icon;
};

struct EditorTextures {
        static void
        Init();

        static void
        Deinitialize();

        static NodeIcons node_icons;
        static FileIcons file_icons;
};

}  // namespace Borek

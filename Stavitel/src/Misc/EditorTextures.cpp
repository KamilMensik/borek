// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Misc/EditorTextures.h"
#include "Include/Engine/Assets/TexAsset.h"
namespace Borek {

void
EditorTextures::Init()
{
        node_icons.node.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Node.png"));
        node_icons.area.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Area.png"));
        node_icons.camera.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Camera.png"));
        node_icons.dynamic_body.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/CharacterBody.png"));
        node_icons.static_body.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/StaticBody.png"));
        node_icons.sprite.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Sprite.png"));
        node_icons.text.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Text.png"));
        node_icons.tilemap.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Tilemap.png"));
        node_icons.sound.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Sound.png"));

        file_icons.folder_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FolderBasic.png"));
        file_icons.basic_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileBasic.png"));
        file_icons.important_folder_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FolderImportant.png"));
        file_icons.config_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileConfig.png"));
        file_icons.image_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileImage.png"));
        file_icons.key_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileKey.png"));
        file_icons.object_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileObject.png"));
        file_icons.script_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileScript.png"));
        file_icons.sound_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileSound.png"));
        file_icons.text_file_icon.LoadFrom(ASSET_PATH("assets/EditorIcons/FileText.png"));
}

void
EditorTextures::Deinitialize()
{
        node_icons.node = TexAsset();
        node_icons.area = TexAsset();
        node_icons.camera = TexAsset();
        node_icons.dynamic_body = TexAsset();
        node_icons.static_body = TexAsset();
        node_icons.sprite = TexAsset();
        node_icons.text = TexAsset();
        node_icons.tilemap = TexAsset();
        node_icons.sound = TexAsset();

        file_icons.folder_icon = TexAsset();
        file_icons.basic_file_icon = TexAsset();
        file_icons.important_folder_icon = TexAsset();
        file_icons.config_file_icon = TexAsset();
        file_icons.image_file_icon = TexAsset();
        file_icons.key_file_icon = TexAsset();
        file_icons.object_file_icon = TexAsset();
        file_icons.script_file_icon = TexAsset();
        file_icons.sound_file_icon = TexAsset();
        file_icons.text_file_icon = TexAsset();
}

NodeIcons EditorTextures::node_icons;
FileIcons EditorTextures::file_icons;

}  // namespace Borek

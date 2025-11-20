// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Misc/EditorTextures.h"
#include <Include/Engine/Assets/TexAsset.h>

#include "NodeIcons.h"

namespace Borek {

const Ref<Graphics::Texture2D>
get_node_type_icon(NodeType type)
{
        switch (type) {
        case NodeType::Node:
                return EditorTextures::node_icons.node;
        case NodeType::Area:
                return EditorTextures::node_icons.area;
        case NodeType::Camera:
                return EditorTextures::node_icons.camera;
        case NodeType::DynamicBody:
                return EditorTextures::node_icons.dynamic_body;
        case NodeType::StaticBody:
                return EditorTextures::node_icons.static_body;
        case NodeType::Sprite:
                return EditorTextures::node_icons.sprite;
        case NodeType::Text:
                return EditorTextures::node_icons.text;
        case NodeType::Tilemap:
                return EditorTextures::node_icons.tilemap;
        case NodeType::SoundPlayer:
                return EditorTextures::node_icons.sound;
        default:
                return nullptr;
        }
}

}  // namespace Borek

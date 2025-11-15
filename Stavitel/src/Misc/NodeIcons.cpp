// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <Include/Engine/Assets/TexAsset.h>

#include "NodeIcons.h"

namespace Borek {

static struct {
        TexAsset node;
        TexAsset area;
        TexAsset camera;
        TexAsset dynamic_body;
        TexAsset static_body;
        TexAsset sprite;
        TexAsset text;
        TexAsset tilemap;
        TexAsset sound;
} node_icons;

static void
init_node_icons()
{
        static bool initialized = false;
        if (initialized)
                return;

        node_icons.node.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Node.png"));
        node_icons.area.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Area.png"));
        node_icons.camera.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Camera.png"));
        node_icons.dynamic_body.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/CharacterBody.png"));
        node_icons.static_body.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/StaticBody.png"));
        node_icons.sprite.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Sprite.png"));
        node_icons.text.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Text.png"));
        node_icons.tilemap.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Tilemap.png"));
        node_icons.sound.LoadFrom(ASSET_PATH("assets/EditorIcons/Node/Sound.png"));

        initialized = true;
}

const Ref<Graphics::Texture2D>
get_node_type_icon(NodeType type)
{
        init_node_icons();

        switch (type) {
        case NodeType::Node:
                return node_icons.node;
        case NodeType::Area:
                return node_icons.area;
        case NodeType::Camera:
                return node_icons.camera;
        case NodeType::DynamicBody:
                return node_icons.dynamic_body;
        case NodeType::StaticBody:
                return node_icons.static_body;
        case NodeType::Sprite:
                return node_icons.sprite;
        case NodeType::Text:
                return node_icons.text;
        case NodeType::Tilemap:
                return node_icons.tilemap;
        case NodeType::SoundPlayer:
                return node_icons.sound;
        default:
                return nullptr;
        }
}

}  // namespace Borek

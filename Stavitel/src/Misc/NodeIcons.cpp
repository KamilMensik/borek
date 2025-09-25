// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <Include/Engine/Assets/AssetManager.h>

#include "NodeIcons.h"

namespace Borek {

static struct {
        Ref<Graphics::Texture2D> node;
        Ref<Graphics::Texture2D> area;
        Ref<Graphics::Texture2D> camera;
        Ref<Graphics::Texture2D> dynamic_body;
        Ref<Graphics::Texture2D> static_body;
        Ref<Graphics::Texture2D> sprite;
        Ref<Graphics::Texture2D> text;
} node_icons;

static void
init_node_icons()
{
        static bool initialized = false;
        if (initialized)
                return;

        node_icons.node = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/Node.png"));
        node_icons.area = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/Area.png"));
        node_icons.camera = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/Camera.png"));
        node_icons.dynamic_body = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/CharacterBody.png"));
        node_icons.static_body = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/StaticBody.png"));
        node_icons.sprite = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/Sprite.png"));
        node_icons.text = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/Node/Text.png"));

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
        default:
                return nullptr;
        }
}

}  // namespace Borek

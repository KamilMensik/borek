// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "NodeIcons.h"
#include "Misc/FontAwesome.h"

namespace Borek {

const char*
get_node_type_icon(NodeType type)
{
        switch (type) {
        case NodeType::Node:
                return ICON_FA_SQUARE;
        case NodeType::Area:
                return ICON_FA_BORDER_NONE;
        case NodeType::Camera:
                return ICON_FA_VIDEO;
        case NodeType::DynamicBody:
                return ICON_FA_PERSON_RUNNING;
        case NodeType::StaticBody:
                return ICON_FA_BRIDGE;
        case NodeType::Sprite:
                return ICON_FA_IMAGE;
        case NodeType::Text:
                return ICON_FA_T;
        case NodeType::Tilemap:
                return ICON_FA_LAYER_GROUP;
        case NodeType::SoundPlayer:
                return ICON_FA_VOLUME_LOW;
        case NodeType::AnimatedSprite:
                return ICON_FA_IMAGES;
        case NodeType::ParticleEmmiter:
                return ICON_FA_HURRICANE;
        default:
                return ICON_FA_QUESTION;
        }
}

}  // namespace Borek

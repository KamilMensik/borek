// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <array>
#include <string>

namespace Borek {

enum class NodeType {
        Node = 0,
        DynamicBody = 1,
        StaticBody = 2,
        Area = 3,
        Sprite = 4,
        Camera = 5,
        Text = 6,
        Tilemap = 7,
        SoundPlayer = 8,
};

static const std::array<const std::string, 9> node_type_strings = {
        "Node", "DynamicBody", "StaticBody",
        "Area", "Sprite", "Camera", "Text",
        "Tilemap", "SoundPlayer",
};

const std::string& get_node_type_string(NodeType type);

NodeType get_node_type(const std::string& str);

}  // namespace Borek

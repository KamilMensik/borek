// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <unordered_map>

#include "Include/Base/Node.h"
#include "Include/Core.h"

namespace Borek {

static std::unordered_map<std::string, NodeType> node_type_map;

static void
init_node_type_map()
{
        static bool initialized = false;
        if (initialized)
                return;

        for (int i = 0; auto item : node_type_strings) {
                node_type_map[item] = SCAST<NodeType>(i);
                i++;
        }

        initialized = true;
}

const std::string&
get_node_type_string(NodeType type)
{
        return node_type_strings[SCAST<int>(type)];
}

NodeType get_node_type(const std::string& str)
{
        init_node_type_map();

        return node_type_map[str];
}

}  // namespace Borek

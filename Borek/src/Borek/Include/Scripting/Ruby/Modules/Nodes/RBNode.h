// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Node.h"
#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"
#include <cstdint>

#define NODE_MODULE_METHOD(_name)                       \
static MRB_FUNC(M##_name)                               \
{                                                       \
        return _name(mrb, MRB_GET_IV(self, "@_node"));  \
}

namespace Borek {
namespace RBModules {

class RBNode : RBModule {
public:
        struct Data {
                mrb_value children;
                mrb_value components;
                uint32_t parent_id;
                uint32_t entity_id;
        };
        static mrb_value Initialize(mrb_state* mrb, mrb_value self);
        static void Init(class RubyEngine& vm);
        static mrb_value NewNode(mrb_state* mrb, mrbcpp::Class& cls,
                                 NodeType type);

        static mrbcpp::Module node_module;
        static mrbcpp::Class node_class;
};

}  // namespace RBModules
}  // namespace Borek

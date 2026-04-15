// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBDynamicBody : RBModule {
public:
        static void Init(RubyEngine& vm);
        static mrbcpp::Module dynamic_body_module;
        static mrbcpp::Class dynamic_body_class;
        static mrbcpp::Class collision_data_class;
};

}  // namespace RBModules
}  // namespace Borek

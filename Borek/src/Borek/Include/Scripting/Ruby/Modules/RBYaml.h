// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBYaml : RBModule {
public:
        static void Init(class RubyEngine& vm);

        static mrbcpp::Class yaml_class;
        static mrbcpp::Class json_class;
};

}  // namespace RBModules
}  // namespace Borek

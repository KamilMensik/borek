// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBArea : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Module area_module;
        static mrbcpp::Class area_class;
};

}  // namespace RBModules
}  // namespace Borek

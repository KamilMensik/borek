// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBText2D : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Module text2d_module;
        static mrbcpp::Class text2d_class;
};

}  // namespace RBModules
}  // namespace Borek

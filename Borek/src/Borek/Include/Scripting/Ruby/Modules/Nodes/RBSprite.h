// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBSprite : RBModule {
public:
        static void Init(RubyEngine& vm);
        static mrbcpp::Module sprite_module;
        static mrbcpp::Class sprite_class;
};

}  // namespace RBModules
}  // namespace Borek

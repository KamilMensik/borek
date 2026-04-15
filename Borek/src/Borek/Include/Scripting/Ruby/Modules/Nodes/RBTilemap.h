// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBTilemap : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Module tilemap_module;
        static mrbcpp::Class tilemap_class;
};

}  // namespace RBModules
}  // namespace Borek

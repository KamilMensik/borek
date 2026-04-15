// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBNDatatypes : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Class vec2_class;
        static mrbcpp::Class vec3_class;
        static mrbcpp::Class vec4_class;
};

}  // namespace RBModules
}  // namespace Borek

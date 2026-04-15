// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

struct mrb_state;

namespace Borek {
namespace RBModules {

class RBDatatypes : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Class vec_class;
        static mrbcpp::Class vec2_class;
        static mrbcpp::Class vec3_class;
        static mrbcpp::Class vec4_class;

        static mrbcpp::Class ivec_class;
        static mrbcpp::Class ivec2_class;
        static mrbcpp::Class ivec3_class;
        static mrbcpp::Class ivec4_class;
};

}  // namespace RBModules
}  // namespace Borek

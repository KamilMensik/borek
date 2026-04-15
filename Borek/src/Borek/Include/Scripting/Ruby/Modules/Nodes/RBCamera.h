// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBCamera : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Module camera_module;
        static mrbcpp::Class camera_class;
};

}  // namespace RBModules
}  // namespace Borek

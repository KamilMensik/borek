// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

struct RBClass;

namespace Borek {
namespace RBModules {

class RBTransform : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Class transform_class;
};

}  // namespace RBModules
}  // namespace Borek

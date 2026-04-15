// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBParticleEmmiter : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Module particle_emmiter_module;
        static mrbcpp::Class particle_emmiter_class;
};

}  // namespace RBModules
}  // namespace Borek

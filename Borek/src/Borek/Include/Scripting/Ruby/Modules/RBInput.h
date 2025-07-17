// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"

namespace Borek {
namespace RBModules {

class Input : RBModule {
public:
        static void Init(class RubyEngine& vm);
};

}  // namespace RBModules
}  // namespace Borek

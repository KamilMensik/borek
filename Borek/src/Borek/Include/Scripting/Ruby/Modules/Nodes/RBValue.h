// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"

namespace Borek {
namespace RBModules {

class RBValue : RBModule {
public:
        static void InitAccessors(mrb_value self);
};

}  // namespace RBModules
}  // namespace Borek

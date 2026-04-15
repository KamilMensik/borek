// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBIter : RBModule {
public:
        struct Data {
                mrb_value obj;
                mrb_value block;
                mrb_sym fsym;
                int index;
        };

        static void Init(class RubyEngine& vm);

        static mrbcpp::Class iter_class;
};

}  // namespace RBModules
}  // namespace Borek

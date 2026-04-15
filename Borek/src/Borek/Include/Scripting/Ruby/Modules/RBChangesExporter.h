// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

enum ChangesExporterTypes {
        kTransform,
};

class ChangesExporter : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static void AddTransform(mrb_state* mrb, mrb_value node);
        static void AddSprite(mrb_state* mrb, mrb_value node);
        static void AddAnimatedSprite(mrb_state* mrb, mrb_value node);
        static void Upload(mrb_state* mrb);
        static void Reset(mrb_state* mrb);
        static mrbcpp::Class exporter_class;
};

}  // namespace RBModules
}  // namespace Borek

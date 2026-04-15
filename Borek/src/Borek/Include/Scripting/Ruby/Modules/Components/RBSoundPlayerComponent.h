// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"

namespace Borek {
namespace RBModules {

class RBSoundPlayerComponent : RBModule {
public:
        static void Init(class RubyEngine& vm);
        static mrbcpp::Class sound_player_component_class;
};

}  // namespace RBModules
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "Include/Scripting/Ruby/Modules/Components/RBAnimatedSpriteComponent.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTagComponent.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTransform.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBAnimatedSprite.h"
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBComponent.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSpriteComponent.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSoundPlayerComponent.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

void Component::Init(RubyEngine& engine)
{
        RBTransform::Init(engine);
        RBSpriteComponent::Init(engine);
        RBSoundPlayerComponent::Init(engine);
        RBAnimatedSpriteComponent::Init(engine);
        RBTagComponent::Init(engine);
}

}  // namespace RBModules
}  // namespace Borek

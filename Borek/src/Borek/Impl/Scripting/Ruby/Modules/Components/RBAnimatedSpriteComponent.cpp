// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>

#include <mrbcpp.h>

#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Components/RBAnimatedSpriteComponent.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        mrb_value node = MRB_ARG1;
        std::array<mrb_value, 3> args;

        Entity e(mrb_integer(MRB_GET_IV(node, "@entity_id")));
        auto& sprite = e.GetComponent<AnimatedSpriteComponent>();
        args = { self, MRB_NUM(sprite.size_x), MRB_NUM(sprite.size_y) };

        MRB_SET_IV(self, "@size", mrb_class_new_instance(
                mrb, 3, args.data(), RBNDatatypes::vec2_class));

        MRB_SET_IV(self, "@flip_x", mrb_false_value());
        MRB_SET_IV(self, "@flip_y", mrb_false_value());
        MRB_SET_IV(self, "@current_anim", mrb_nil_value());
        MRB_SET_IV(self, "@node", node);
        
        return self;
}

static MRB_FUNC(Notify)
{
        ChangesExporter::AddAnimatedSprite(mrb, MRB_GET_IV(self, "@node"));
        return MRB_NIL;
}

void RBAnimatedSpriteComponent::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        sprite_component_class = borek.define_class("AnimatedSpriteComponent")
                .define_method("initialize", Initialize,
                               FuncArgs().Required(1))
                .define_method("notify", Notify);
}

Class RBAnimatedSpriteComponent::sprite_component_class;

}  // namespace RBModules
}  // namespace Borek

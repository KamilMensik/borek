// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Debug/Log.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSprite.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSpriteComponent.h"

#define GET_SPRITE_SAFE                                                        \
mrb_value sprite = MRB_GET_IV(self, "@_sprite");                               \
if (mrb_nil_p(sprite)) {                                                       \
        sprite = mrb_class_new_instance(mrb, 1, &self, RBSpriteComponent::sprite_component_class);\
        MRB_SET_IV(self, "@_sprite", sprite);                                  \
}

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(FlipX)
{
        GET_SPRITE_SAFE;
        return MRB_GET_IV(sprite, "@flip_x");
}

static MRB_FUNC(FlipY)
{
        GET_SPRITE_SAFE;
        return MRB_GET_IV(sprite, "@flip_y");
}

static MRB_FUNC(SetFlipX)
{
        GET_SPRITE_SAFE;
        MRB_SET_IV(sprite, "@flip_x", MRB_ARG1);
        ChangesExporter::AddSprite(mrb, self);

        return MRB_NIL;
}

static MRB_FUNC(SetFlipY)
{
        GET_SPRITE_SAFE;
        MRB_SET_IV(sprite, "@flip_y", MRB_ARG1);
        ChangesExporter::AddSprite(mrb, self);

        return MRB_NIL;
}

static MRB_FUNC(Size)
{
        GET_SPRITE_SAFE;
        return MRB_GET_IV(sprite, "@size");
}

static MRB_FUNC(SetSize)
{
        GET_SPRITE_SAFE;
        mrb_value size = MRB_GET_IV(sprite, "@size");
        auto data = MRB_ISTRUCT_VAL(size, glm::vec2);
        mrb_value arg = MRB_ARG1;
        if (mrb_type(arg) != MRB_TT_ISTRUCT) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }
        auto stru = RCAST<RIStruct*>(arg.w);
        if (stru->c != RBDatatypes::vec2_class &&
            stru->c != RBNDatatypes::vec2_class) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }

        *data = *RCAST<glm::vec2*>(stru->inline_data);

        ChangesExporter::AddSprite(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBSprite::sprite_class, NodeType::Sprite);
}

NODE_MODULE_METHOD(FlipX);
NODE_MODULE_METHOD(FlipY);
NODE_MODULE_METHOD(SetFlipX);
NODE_MODULE_METHOD(SetFlipY);
NODE_MODULE_METHOD(Size);
NODE_MODULE_METHOD(SetSize);

void
RBSprite::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        sprite_class = vm.define_class("SpriteC", RBNode::node_class)
                .define_method("flip_x", FlipX)
                .define_method("flip_y", FlipY)
                .define_method("flip_x=", SetFlipX, FuncArgs().Required(1))
                .define_method("flip_y=", SetFlipY, FuncArgs().Required(1))
                .define_method("size", Size)
                .define_method("size=", SetSize, FuncArgs().Required(1));

        sprite_module = vm.define_module("Sprite")
                .include(RBNode::node_module)
                .define_method("flip_x", MFlipX)
                .define_method("flip_y", MFlipY)
                .define_method("flip_x=", MSetFlipX, FuncArgs().Required(1))
                .define_method("flip_y=", MSetFlipY, FuncArgs().Required(1))
                .define_method("size", MSize)
                .define_method("size=", MSetSize, FuncArgs().Required(1))
                .define_module_method("new", New);
}

Module RBSprite::sprite_module;
Class RBSprite::sprite_class;

}  // namespace RBModules
}  // namespace vm

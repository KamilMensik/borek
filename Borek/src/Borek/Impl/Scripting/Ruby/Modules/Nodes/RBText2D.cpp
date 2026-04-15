// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mrbcpp.h>

#include "Include/Core.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBText2D.h"
#include "Include/Components/Text2DComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include "Include/Engine/Assets/FontAsset.h"
#include "Include/Engine/Assets/Asset.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Text)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        return MRB_STRING(e.GetComponent<Text2DComponent>().value.Str().c_str());
}

static MRB_FUNC(SetText)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        auto& txt = e.GetComponent<Text2DComponent>();

        txt.value = mrb_string_cstr(mrb, MRB_ARG1);

        return MRB_NIL;
}

static MRB_FUNC(SetFont)
{
        mrb_value arg = MRB_ARG1;
        ASSET_TYPE_ASSERT(arg, font);

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        Asset<FontAsset> ass(SCAST<uint32_t>(RCAST<uint64_t>(DATA_PTR(arg))));
        e.GetComponent<Text2DComponent>().font = ass;
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBText2D::text2d_class, NodeType::Text);
}

NODE_MODULE_METHOD(Text);
NODE_MODULE_METHOD(SetText);
NODE_MODULE_METHOD(SetFont);

void
RBText2D::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        text2d_class = vm.define_class("Text2DC", RBNode::node_class)
                .define_method("text", Text)
                .define_method("text=", SetText, FuncArgs().Required(1))
                .define_method("font=", SetFont, FuncArgs().Required(1));

        text2d_module = vm.define_module("Text2D")
                .include(RBNode::node_module)
                .define_method("text", MText)
                .define_method("text=", MSetText, FuncArgs().Required(1))
                .define_method("font=", MSetFont, FuncArgs().Required(1))
                .define_module_method("new", New);
}

Module RBText2D::text2d_module;
Class RBText2D::text2d_class;

}  // namespace RBModules
}  // namespace vm

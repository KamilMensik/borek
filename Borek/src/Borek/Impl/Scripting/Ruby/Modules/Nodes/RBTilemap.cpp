// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/value.h>
#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBTilemap.h"
#include "Include/Base/Entity.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/TilemapAsset.h"
#include "Include/Engine/Assets/Asset.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(SetAsset)
{
        mrb_value arg = MRB_ARG1;
        ASSET_TYPE_ASSERT(arg, tilemap);

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        Asset<TilemapAsset> ass(SCAST<uint32_t>(RCAST<uint64_t>(DATA_PTR(arg))));
        e.GetComponent<TilemapComponent>().tilemap = ass;
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBTilemap::tilemap_class, NodeType::Tilemap);
}

static MRB_FUNC(Get)
{
        mrb_int row, col;
        mrb_get_args(mrb, "ii", &row, &col);

        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto& tc = e.GetComponent<TilemapComponent>();
        auto it = tc.items.find({row, col});
        if (it == tc.items.end())
                return MRB_NIL;

        return MRB_NUM(it->second);
}

static MRB_FUNC(Set)
{
        mrb_int row, col;
        mrb_value item;

        mrb_get_args(mrb, "iio", &row, &col, &item);

        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto& tc = e.GetComponent<TilemapComponent>();

        if (mrb_nil_p(item)) {
                tc.Delete(e, col, row);
        } else {
                tc.Set(e, col, row, mrb_integer(item));
        }

        return MRB_NIL;
}

NODE_MODULE_METHOD(SetAsset);

void
RBTilemap::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        tilemap_class = vm.define_class("TilemapC", RBNode::node_class)
                .define_method("asset=", SetAsset, FuncArgs().Required(1))
                .define_method("[]", Get, FuncArgs().Required(2))
                .define_method("[]=", Set, FuncArgs().Required(3));

        tilemap_module = vm.define_module("Tilemap")
                .include(RBNode::node_module)
                .define_method("asset=", MSetAsset, FuncArgs().Required(1))
                .define_method("[]", Get, FuncArgs().Required(2))
                .define_method("[]=", Set, FuncArgs().Required(3))
                .define_module_method("new", New);
}

Module RBTilemap::tilemap_module;
Class RBTilemap::tilemap_class;

}  // namespace RBModules
}  // namespace vm

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
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
#include "Include/Scripting/Ruby/Modules/Nodes/RBArea.h"
#include "Include/Base/Entity.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/RBMsgConnection.h"

#define ADD_CALLBACK(_obj, _name, _callback)                                   \
{                                                                              \
        auto callback = mrb_class_new_instance(                                \
                mrb, 0, nullptr, RBMsgConnection::msg_connection_class);       \
                                                                               \
        auto data = MRB_ISTRUCT_VAL(callback, RBMsgConnection::Data);          \
        data->connection = _callback;                                          \
        MRB_SET_IV(_obj, "@"#_name, callback);                                 \
}

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        RBNode::Initialize(mrb, self);
        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto& area = e.GetComponent<AreaComponent>();

        ADD_CALLBACK(self, "on_area_entered", &area.callbacks->on_area_entered);
        ADD_CALLBACK(self, "on_area_exited", &area.callbacks->on_area_exited);
        ADD_CALLBACK(self, "on_body_entered", &area.callbacks->on_body_entered);
        ADD_CALLBACK(self, "on_body_exited", &area.callbacks->on_body_exited);

        return self;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBArea::area_class, NodeType::Area);
}

static MRB_FUNC(OnAreaEntered)
{
        return MRB_GET_IV(self, "@on_area_entered");
}

static MRB_FUNC(OnAreaExited)
{
        return MRB_GET_IV(self, "@on_area_exited");
}

static MRB_FUNC(OnBodyEntered)
{
        return MRB_GET_IV(self, "@on_body_entered");
}

static MRB_FUNC(OnBodyExited)
{
        return MRB_GET_IV(self, "@on_body_exited");
}

void
RBArea::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        area_class = vm.define_class("AreaC", RBNode::node_class)
                .define_method("initialize", Initialize)
                .define_method("on_area_entered", OnAreaEntered)
                .define_method("on_area_exited", OnAreaExited)
                .define_method("on_body_entered", OnBodyEntered)
                .define_method("on_body_exited", OnBodyExited);

        area_module = vm.define_module("Area")
                .include(RBNode::node_module)
                .define_method("on_area_entered", OnAreaEntered)
                .define_method("on_area_exited", OnAreaExited)
                .define_method("on_body_entered", OnBodyEntered)
                .define_method("on_body_exited", OnBodyExited)
                .define_module_method("new", New);
}

Module RBArea::area_module;
Class RBArea::area_class;

}  // namespace RBModules
}  // namespace vm

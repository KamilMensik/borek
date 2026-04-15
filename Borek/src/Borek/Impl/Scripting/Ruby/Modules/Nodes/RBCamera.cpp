// Copyright 2024-2025 <kamilekmensik@gmail.com>

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
#include "Include/Scripting/Ruby/Modules/Nodes/RBCamera.h"
#include "Include/Base/Entity.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Base/Query.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Activate)
{
        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto& cam = e.GetComponent<CameraComponent>();
        if (cam.is_active)
                return mrb_false_value();

        for (auto& [cam] : Query<CameraComponent>()) {
                cam->is_active = false;
        }

        cam.is_active = true;
        return mrb_true_value();
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBCamera::camera_class, NodeType::Area);
}

void
RBCamera::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        camera_class = vm.define_class("CameraC", RBNode::node_class)
                .define_method("activate", Activate);

        camera_module = vm.define_module("Camera")
                .include(RBNode::node_module)
                .define_method("activate", Activate)
                .define_module_method("new", New);
}

Module RBCamera::camera_module;
Class RBCamera::camera_class;

}  // namespace RBModules
}  // namespace vm

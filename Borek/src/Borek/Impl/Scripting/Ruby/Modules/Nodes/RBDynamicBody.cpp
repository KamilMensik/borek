// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/string.h>

#include <mrbcpp.h>

#include "Include/Engine/FZX/Collision.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBDynamicBody.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTransform.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"

namespace Borek {
namespace RBModules {

struct CollisionData {
        FZX::Collision col;
};

static mrb_value
new_cdata(mrb_state* mrb, const FZX::Collision& col);

using namespace mrbcpp;

static MRB_FUNC(MoveAndCollide)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto val = MRB_ISTRUCT_VAL(MRB_ARG1, glm::vec2);

        const auto res = e.MoveAndCollide(val->x, val->y);
        *val = res.second;

        TransformComponent& tc = e.Transform();
        mrb_value tval = MRB_GET_IV(self, "@_transform");
        if (mrb_nil_p(tval)) {
                tval = mrb_class_new_instance(mrb, 1, &self,
                                              RBTransform::transform_class);
                MRB_SET_IV(self, "@_transform", tval);
        }

        auto pos = MRB_ISTRUCT_VAL(MRB_GET_IV(tval, "@position"), glm::vec2);
        pos->x = tc.position.x;
        pos->y = tc.position.y;

        return new_cdata(mrb, res.first);
}

static MRB_FUNC(IsOnFloor)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));

        bool res = e.GetComponent<BodyComponent>().physics_flags & FZX::PhysicsFlags_CollidedBottom;

        return mrb_bool_value(res);
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBDynamicBody::dynamic_body_class, NodeType::DynamicBody);
}

NODE_MODULE_METHOD(MoveAndCollide);
NODE_MODULE_METHOD(IsOnFloor);

static mrb_value
new_cdata(mrb_state* mrb, const FZX::Collision& col)
{
        if (!col)
                return MRB_NIL;

        mrb_value cdata = mrb_class_new_instance(
                mrb, 0, nullptr, RBDynamicBody::collision_data_class);

        MRB_ISTRUCT_VAL(cdata, CollisionData)->col = col;

        return cdata;
}

static MRB_FUNC(CDataGetObj)
{
        Entity e(MRB_ISTRUCT_VAL(self, CollisionData)->col.collider);

        return e.IsNil() ? MRB_NIL : mrb_value{e.GetRubyNode()};
}

static MRB_FUNC(CDataGetNormal)
{
        const auto& data = MRB_ISTRUCT_VAL(self, CollisionData)->col;
        if (!data)
                return mrb_class_new_instance(mrb, 0, nullptr, RBDatatypes::ivec2_class);

        mrb_value args[2] = { MRB_FLOAT(data.normal.x), MRB_FLOAT(data.normal.y) };
        return mrb_class_new_instance(mrb, 2, args, RBDatatypes::ivec2_class);
}

static MRB_FUNC(CDataGetData)
{
        const FZX::Collision& col = MRB_ISTRUCT_VAL(self, CollisionData)->col;
        Entity e(col.collider);
        if (e.IsNil()) return MRB_NIL;

        switch (e.GetNodeType()) {
        case NodeType::Tilemap: {
                mrb_value args[2] = { MRB_NUM(col.tilemap_data.row),
                                      MRB_NUM(col.tilemap_data.col) };

                return mrb_class_new_instance(mrb, 2, args, RBDatatypes::ivec2_class);
        }
        default:
                return MRB_NIL;
        }

        return { e.GetRubyNode() };
}

void
RBDynamicBody::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        dynamic_body_class = vm.define_class("DynamicBodyC", RBNode::node_class)
                .define_method("move_and_collide", MoveAndCollide, FuncArgs().Required(1))
                .define_method("on_floor?", IsOnFloor);

        dynamic_body_module = vm.define_module("DynamicBody")
                .include(RBNode::node_module)
                .define_method("move_and_collide", MMoveAndCollide, FuncArgs().Required(1))
                .define_method("on_floor?", MIsOnFloor)
                .define_module_method("new", New);

        collision_data_class = vm.define_class("CollisionData", MRB_TT_ISTRUCT)
                .define_method("obj", CDataGetObj)
                .define_method("normal", CDataGetNormal)
                .define_method("data", CDataGetData);
}

Module RBDynamicBody::dynamic_body_module;
Class RBDynamicBody::dynamic_body_class;
Class RBDynamicBody::collision_data_class;

}  // namespace RBModules
}  // namespace Borek

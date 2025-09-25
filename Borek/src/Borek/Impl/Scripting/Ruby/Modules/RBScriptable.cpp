// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/hash.h>
#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBScriptable.h"
#include "Include/Base/Entity.h"
#include "Include/Engine/FZX/Body.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(OnCreate)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnUpdate)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnDestroy)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnEvent)
{
        return mrb_nil_value();
}

static MRB_FUNC(GetComponent)
{
        return MRB_FUNCALL(MRB_GET_IV(self, "@entity"), "get_component", MRB_ARG1);
}

static MRB_FUNC(GetTransform)
{
        return MRB_FUNCALL(MRB_GET_IV(self, "@entity"), "transform");
}

static MRB_FUNC(MoveAndCollide)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(MRB_GET_IV(self, "@entity"),
                                               "@id")));

        mrb_value vec = MRB_ARG1;
        float x = mrb_float(MRB_GET_IV(vec, "@x"));
        float y = mrb_float(MRB_GET_IV(vec, "@y"));

        glm::vec2 res = e.MoveAndCollide(x, y);

        MRB_SET_IV(vec, "@x", MRB_FLOAT(res.x));
        MRB_SET_IV(vec, "@y", MRB_FLOAT(res.y));

        return self;
}

static MRB_FUNC(IsOnFloor)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(MRB_GET_IV(self, "@entity"),
                                               "@id")));
        bool res = e.GetComponent<FZX::BodyComponent>().physics_flags & FZX::PhysicsFlags_CollidedBottom;

        return mrb_bool_value(res);
}

static MRB_FUNC(Initialize)
{
        return self;
}

void Scriptable::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        borek.define_class("Scriptable")
             .define_method("initialize", Initialize, FuncArgs().Required(1))
             .define_method("on_create", OnCreate)
             .define_method("on_update", OnUpdate)
             .define_method("on_destroy", OnDestroy)
             .define_method("on_event", OnEvent)
             .define_method("get_component", GetComponent,
                            FuncArgs().Required(1))
             .define_method("move_and_collide", MoveAndCollide, FuncArgs().Required(1))
             .define_method("on_floor?", IsOnFloor)
             .define_method("transform", GetTransform)
             .define_attr_accessor<"entity">();
}

}  // namespace RBModules
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Debug/Log.h"
#include <cstdint>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/class.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBEntity.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        mrb_uint id = UINT32_MAX;
        
        mrb_get_args(mrb, "|i", &id);
        MRB_SET_IV(self, "@id", MRB_NUM(id));

        return self;
}

static MRB_FUNC(Equal)
{
        bool res = mrb_integer(MRB_GET_IV(self, "@id")) == mrb_fixnum(MRB_GET_IV(MRB_ARG1, "@id"));
        return mrb_bool_value(res);
}

static MRB_FUNC(GetComponent)
{
        mrb_value id = MRB_GET_IV(self, "@id");
        return mrb_class_new_instance(mrb, 1, &id, mrb_class_ptr(MRB_ARG1));
}

static MRB_FUNC(GetTransform)
{
        VM vm(mrb);
        RClass* trans_c = mrb_class_get_under(mrb, mrb_module_get(mrb, "Borek"), "TransformComponent");
        mrb_value tc = vm.value_from(trans_c);

        return MRB_FUNCALL(self, "get_component", tc);
}

static MRB_FUNC(AddComponent)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@id")));
        e.AddComponent(mrb_integer(MRB_CONST_GET(MRB_ARG1, "COMPONENT_ID")));
        return self;
}

static MRB_FUNC(RemoveComponent)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@id")));
        e.RemoveComponent(mrb_integer(MRB_CONST_GET(MRB_ARG1, "COMPONENT_ID")));
        return self;
}

static MRB_FUNC(Delete)
{
        Borek::Entity(mrb_integer(MRB_GET_IV(self, "@id"))).Delete();
        return MRB_NIL;
}

static MRB_FUNC(HasComponent)
{
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@id")));
        return mrb_bool_value(e.HasComponent(mrb_integer(MRB_CONST_GET(MRB_ARG1, "COMPONENT_ID"))));
}

static MRB_FUNC(IsNil)
{
        BOREK_LOG_INFO("Entity id is {}", mrb_integer(MRB_GET_IV(self, "@id")));
        return mrb_bool_value(mrb_integer(MRB_GET_IV(self, "@id")) == UINT32_MAX);
}

static MRB_FUNC(ToS)
{
        return MRB_STRING(std::format("Borek::Entity {}",
                                      mrb_integer(MRB_GET_IV(self, "@id"))).c_str());
}

static MRB_FUNC(Move)
{
        mrb_float x, y;
        
        mrb_get_args(mrb, "ff", &x, &y);
        Borek::Entity e(mrb_integer(MRB_GET_IV(self, "@id")));
        e.MoveAndCollide(x, y);

        return self;
}

void Entity::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        borek.define_class("Entity")
                .define_method("initialize", Initialize, FuncArgs().Optional(1))
                .define_method("==", Equal, FuncArgs().Required(1))
                .define_method("get_component", GetComponent, FuncArgs().Required(1))
                .define_method("add_component", AddComponent, FuncArgs().Required(1))
                .define_method("remove_component", RemoveComponent, FuncArgs().Required(1))
                .define_method("has_component?", HasComponent, FuncArgs().Required(1))
                .define_method("transform", GetTransform)
                .define_method("nil?", IsNil)
                .define_method("delete", Delete)
                .define_method("to_s", ToS)
                .define_method("move", Move, FuncArgs().Required(2))
                .define_attr_accessor<"id">();

}

}  // namespace RBModules
}  // namespace Borek

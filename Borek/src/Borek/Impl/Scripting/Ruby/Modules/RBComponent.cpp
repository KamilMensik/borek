// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "ECS/Component.h"
#include "Include/Base/Application.h"
#include "Include/Base/Components.h"

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBComponent.h"
#include "Include/Core.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static const mrb_data_type component_type = {
        "Component", mrb_free
};

static Class vec2;
//static Class vec3;
//static Class vec4;

static Class transform_position;
static Class transform_scale;


MRB_FUNC(ComponentInitialize)
{
        mrb_int entity_id;
        mrb_get_args(mrb, "i", &entity_id);
        
        Entity* entity = RCAST<Entity*>(malloc(sizeof(Entity)));
        entity->m_Id = entity_id;
        entity->m_Scene = Application::GetScene().get();

        mrb_data_init(self, entity, &component_type);
        
        return self;
}

MRB_FUNC(TransformPositionInitialize)
{
        mrb_value transform;
        mrb_float x = 0;
        mrb_float y = 0;
        mrb_get_args(mrb, "o|ff", &transform, &x, &y);

        MRB_SET_IV(self, "@transform", transform);
        MRB_SET_IV(self, "@x", MRB_FLOAT(x));
        MRB_SET_IV(self, "@y", MRB_FLOAT(y));

        return self;
}

MRB_FUNC(TransformPositionSetX)
{
        MRB_SET_IV(self, "@x", MRB_ARG1);
        MRB_FUNCALL(MRB_GET_IV(self, "@transform"), "position=", self);
        return MRB_ARG1;
}

MRB_FUNC(TransformPositionSetY)
{
        MRB_SET_IV(self, "@y", MRB_ARG1);
        MRB_FUNCALL(MRB_GET_IV(self, "@transform"), "position=", self);
        return MRB_ARG1;
}

MRB_FUNC(TransformScaleInitialize)
{
        mrb_value transform;
        mrb_float x = 0;
        mrb_float y = 0;
        mrb_get_args(mrb, "o|ff", &transform, &x, &y);

        MRB_SET_IV(self, "@transform", transform);
        MRB_SET_IV(self, "@x", MRB_FLOAT(x));
        MRB_SET_IV(self, "@y", MRB_FLOAT(y));

        return self;
}

MRB_FUNC(TransformScaleSetX)
{
        MRB_SET_IV(self, "@x", MRB_ARG1);
        MRB_FUNCALL(MRB_GET_IV(self, "@transform"), "scale=", self);
        return MRB_ARG1;
}

MRB_FUNC(TransformScaleSetY)
{
        MRB_SET_IV(self, "@y", MRB_ARG1);
        MRB_FUNCALL(MRB_GET_IV(self, "@transform"), "scale=", self);
        return MRB_ARG1;
}

MRB_FUNC(TransformGetPosition)
{
        Entity *e;
        mrb_value val[3];

        Data_Get_Struct(mrb, self, &component_type, e);
        TransformComponent& trans = e->GetComponent<TransformComponent>();

        val[0] = self;
        val[1] = MRB_FLOAT(trans.position.x * 400);
        val[2] = MRB_FLOAT(trans.position.y * 400);

        return mrb_class_new_instance(mrb, 3, val, transform_position);
}

MRB_FUNC(TransformSetPosition)
{
        Entity *e;
        mrb_value position = MRB_ARG1;

        Data_Get_Struct(mrb, self, &component_type, e);
        TransformComponent& trans = e->GetComponent<TransformComponent>();

        trans.position.x = mrb_float(MRB_GET_IV(position, "@x")) / 400;
        trans.position.y = mrb_float(MRB_GET_IV(position, "@y")) / 400;

        return self;
}

MRB_FUNC(TransformGetScale)
{
        Entity *e;
        mrb_value val[3];

        Data_Get_Struct(mrb, self, &component_type, e);
        TransformComponent& trans = e->GetComponent<TransformComponent>();

        val[0] = self;
        val[1] = MRB_FLOAT(trans.scale.x * 400);
        val[2] = MRB_FLOAT(trans.scale.y * 400);

        return mrb_class_new_instance(mrb, 3, val, transform_scale);
}

MRB_FUNC(TransformSetScale)
{
        Entity *e;
        mrb_value position = MRB_ARG1;

        Data_Get_Struct(mrb, self, &component_type, e);
        TransformComponent& trans = e->GetComponent<TransformComponent>();

        trans.position.x = mrb_float(MRB_GET_IV(position, "@x")) / 400;
        trans.position.y = mrb_float(MRB_GET_IV(position, "@y")) / 400;

        return self;
}

void Component::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        VM vm = engine.GetRubyVM();

        // Setup of internal static variables in order to run functions faster.
        vec2 = Class(vm, mrb_class_get_under(vm, borek, "Vec2"));
        //vec3 = mrb_class_get(vm, "Borek::Vec3");
        //vec4 = mrb_class_get(vm, "Borek::Vec4");

        transform_position = borek.define_class("TransformPosition", vec2)
                .define_method("initialize", TransformPositionInitialize,
                               FuncArgs().Required(1).Optional(2))
                .define_method("x=", TransformPositionSetX, FuncArgs().Required(1))
                .define_method("y=", TransformPositionSetY, FuncArgs().Required(1));

        transform_scale = borek.define_class("TransformScale", vec2)
                .define_method("initialize", TransformScaleInitialize,
                               FuncArgs().Required(1).Optional(2))
                .define_method("x=", TransformScaleSetX, FuncArgs().Required(1))
                .define_method("y=", TransformScaleSetY, FuncArgs().Required(1));
                
        borek.define_class("TransformComponent")
                .define_method("initialize", ComponentInitialize,
                               FuncArgs().Required(1))
                .define_method("position", TransformGetPosition)
                .define_method("position=", TransformSetPosition, FuncArgs().Required(1))
                .define_method("scale", TransformGetScale)
                .define_method("scale=", TransformSetScale, FuncArgs().Required(1))
                .define_const("COMPONENT_ID", MRB_NUM(ECS::GetId<TransformComponent>()));
}

}  // namespace RBModules
}  // namespace Borek

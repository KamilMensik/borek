// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>

#include <mrbcpp.h>

#include "Include/Core.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSpriteComponent.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static const mrb_data_type component_type = {
        "Component", mrb_free
};

//static Class vec2;
//static Class vec3;
//static Class vec4;


static MRB_FUNC(ComponentInitialize)
{
        mrb_int entity_id;
        mrb_get_args(mrb, "i", &entity_id);
        
        Entity* entity = RCAST<Entity*>(malloc(sizeof(Entity)));
        entity->m_Id = entity_id;

        mrb_data_init(self, entity, &component_type);
        
        return self;
}

static MRB_FUNC(GetFlipX)
{
        Entity *e;

        Data_Get_Struct(mrb, self, &component_type, e);
        const SpriteComponent& spr = e->GetComponent<SpriteComponent>();

        return MRB_BOOL(spr.flags.HasFlags(SpriteComponentFlags_FlipX));
}

static MRB_FUNC(SetFlipX)
{
        Entity *e;

        Data_Get_Struct(mrb, self, &component_type, e);
        SpriteComponent& spr = e->GetComponent<SpriteComponent>();
        
        spr.flags.SetFlags(SpriteComponentFlags_FlipX, mrb_bool(MRB_ARG1));

        return MRB_NIL;
}

static MRB_FUNC(GetFlipY)
{
        Entity *e;

        Data_Get_Struct(mrb, self, &component_type, e);
        const SpriteComponent& spr = e->GetComponent<SpriteComponent>();

        return MRB_BOOL(spr.flags.HasFlags(SpriteComponentFlags_FlipY));
}

static MRB_FUNC(SetFlipY)
{
        Entity *e;

        Data_Get_Struct(mrb, self, &component_type, e);
        SpriteComponent& spr = e->GetComponent<SpriteComponent>();
        
        spr.flags.SetFlags(SpriteComponentFlags_FlipY, mrb_bool(MRB_ARG1));

        return MRB_NIL;
}

void RBSpriteComponent::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        // Setup of internal static variables in order to run functions faster.
        //vec2 = Class(vm, mrb_class_get_under(vm, borek, "Vec2"));
        //vec3 = mrb_class_get(vm, "Borek::Vec3");
        //vec4 = mrb_class_get(vm, "Borek::Vec4");

        borek.define_class("SpriteComponent")
                .define_method("initialize", ComponentInitialize,
                               FuncArgs().Required(1))
                .define_method("flip_x", GetFlipX)
                .define_method("flip_x=", SetFlipX, FuncArgs().Required(1))
                .define_method("flip_y", GetFlipY)
                .define_method("flip_y=", SetFlipY, FuncArgs().Required(1))
                .define_const("COMPONENT_ID", MRB_NUM(ECS::GetId<SpriteComponent>()));
}

}  // namespace RBModules
}  // namespace Borek

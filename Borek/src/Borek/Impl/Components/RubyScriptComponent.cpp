// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mrbcpp.h>

#include "Include/Components/RubyScriptComponent.h"
#include "Include/Base/Application.h"

namespace Borek {

RubyScriptComponent::RubyScriptComponent()
{
}

void RubyScriptComponent::Initialize(Entity e)
{
        if (!script.IsValid())
                return;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        if (!s_RubyEntityClass)
                s_RubyEntityClass = mrb_class_get_under(mrb, Application::GetRubyEngine().GetBorekModule(), "Entity");

        mrb_value entity_id = mrb_fixnum_value(e.GetId());
        mrb_value ruby_entity = mrb_class_new_instance(mrb, 1, &entity_id, s_RubyEntityClass);
        mrb_value instance = mrb_class_new_instance(mrb, 0, nullptr, RCAST<RClass*>(script.Convert().ruby_class));
        MRB_SET_IV(instance, "@entity", ruby_entity);
        MRB_FUNCALL(instance, "on_create");
        
        ruby_instance = instance.w;
}

void RubyScriptComponent::OnUpdate(float delta)
{
        if (!script.IsValid())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_update", MRB_FLOAT(delta));
}

void RubyScriptComponent::OnDestroy()
{
        if (!script.IsValid())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_destroy");
}

RClass* RubyScriptComponent::s_RubyEntityClass = nullptr;

}  // namespace Borek

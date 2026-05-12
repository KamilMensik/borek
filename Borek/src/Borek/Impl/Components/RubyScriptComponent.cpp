// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Exceptions/RubyException.h"
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
        mrb_value entity_id = mrb_fixnum_value(e.GetId());
        mrb_value instance = mrb_class_new_instance(mrb, 0, nullptr, RCAST<RClass*>(script.Convert().ruby_class));
        if (mrb->exc)
                throw RubyException(":(");

        mrb_gc_register(mrb, instance);
        MRB_SET_IV(instance, "@entity_id", entity_id);

        if (mrb->exc)
                throw RubyException(":(");
        
        ruby_instance = instance.w;
}

void RubyScriptComponent::OnUpdate(float delta)
{
        if (!script.IsValid())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_update", MRB_FLOAT(delta));
        if (mrb->exc) {
                throw RubyException("val");
        }
}

void RubyScriptComponent::OnDestroy()
{
        if (!script.IsValid())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_destroy");
        mrb_gc_unregister(mrb, instance);
        if (mrb->exc)
                throw RubyException(":(");
}

}  // namespace Borek

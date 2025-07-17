// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBScriptable.h"

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

static MRB_FUNC(Initialize)
{
        MRB_SET_IV(self, "@entity", MRB_ARG1);
        MRB_FUNCALL(self, "on_create");
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
             .define_method("transform", GetTransform)
             .define_attr_accessor<"entity">();
}

}  // namespace RBModules
}  // namespace Borek

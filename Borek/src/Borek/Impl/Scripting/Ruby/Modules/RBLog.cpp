// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Scripting/Ruby/RubyEngine.h"

#include <mruby.h>
#include <mruby/string.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/Modules/RBLog.h"
#include "Include/Debug/Log.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Info)
{
        BOREK_LOG_INFO("{}", mrb_str_to_cstr(mrb, MRB_ARG1));
        return mrb_nil_value();
}

static MRB_FUNC(Error)
{
        BOREK_LOG_ERROR("{}", mrb_str_to_cstr(mrb, MRB_ARG1));
        return mrb_nil_value();
}

static MRB_FUNC(Fatal)
{
        BOREK_LOG_FATAL("{}", mrb_str_to_cstr(mrb, MRB_ARG1));
        return mrb_nil_value();
}

static MRB_FUNC(Trace)
{
        BOREK_LOG_TRACE("{}", mrb_str_to_cstr(mrb, MRB_ARG1));
        return mrb_nil_value();
}

static MRB_FUNC(Warn)
{
        BOREK_LOG_WARN("{}", mrb_str_to_cstr(mrb, MRB_ARG1));
        return mrb_nil_value();
}

void Log::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        borek.define_class("Log")
                .define_class_method("info", Info, FuncArgs().Required(1))
                .define_class_method("warn", Warn, FuncArgs().Required(1))
                .define_class_method("error", Error, FuncArgs().Required(1))
                .define_class_method("trace", Trace, FuncArgs().Required(1))
                .define_class_method("fatal", Fatal, FuncArgs().Required(1));
}

}  // namespace RBModules
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include <cstdio>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/irep.h>
#include <mruby/dump.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyScript.h"
#include "Include/Base/Application.h"

namespace Borek {

RubyScript::RubyScript(const std::filesystem::path& path)
{
        mrbcpp::VM& vm = Application::GetRubyEngine().GetRubyVM();

        FILE* bytecode = fopen(path.c_str(), "rb");
        int ai = mrb_gc_arena_save((mrb_state*)vm);
        mrb_load_irep_file(vm, bytecode);
        mrb_gc_arena_restore((mrb_state*)vm, ai);

        m_ClassSym = mrb_intern_cstr(vm, path.stem().c_str());
}

}  // namespace Borek

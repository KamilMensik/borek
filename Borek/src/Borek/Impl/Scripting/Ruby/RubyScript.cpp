// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Scripting/Ruby/RubyScript.h"
#include "Include/Base/Application.h"
#include "Include/Debug/Log.h"
#include "mruby/compile.h"
#include <cstdio>
#include <mrbcpp.h>

namespace Borek {

RubyScript::RubyScript(const std::filesystem::path& path)
{
        BOREK_ENGINE_INFO("Loading script {}", path.string());
        mrbcpp::VM vm = Application::GetRubyEngine().GetRubyVM();

        int ai = mrb_gc_arena_save((mrb_state*)vm);
        mrb_load_file(vm, fopen(path.c_str(), "r"));
        mrb_gc_arena_restore((mrb_state*)vm, ai);

        m_ClassSym = mrb_intern_cstr(vm, path.stem().c_str());
}

}  // namespace Borek

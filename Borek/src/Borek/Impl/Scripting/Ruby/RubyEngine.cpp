// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/value.h>
#include <mruby/variable.h>
#include <mruby/string.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Debug/Log.h"
#include "Include/Debug/Assert.h"

#include "Include/Scripting/Ruby/Modules/RBLog.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBComponent.h"
#include "Include/Scripting/Ruby/Modules/RBEntity.h"
#include "Include/Scripting/Ruby/Modules/RBScriptable.h"
#include "Include/Scripting/Ruby/Modules/RBInput.h"
#include "Include/Scripting/Ruby/Modules/RBScene.h"

namespace Borek {

RubyEngine::RubyEngine() : m_RubyVM(mrbcpp::VM()),
        m_BorekModule(m_RubyVM.define_module("Borek")), m_Compiler(m_RubyVM)
{
        InitModules();
        BOREK_ENGINE_INFO("INITIALIZED RUBY ENGINE");
}

RubyEngine::~RubyEngine()
{
        m_RubyVM.Destroy();
}

void RubyEngine::InitModules()
{
        RBModules::Datatypes::Init(*this);
        RBModules::Log::Init(*this);
        RBModules::Entity::Init(*this);
        RBModules::Component::Init(*this);
        RBModules::Scriptable::Init(*this);
        RBModules::Input::Init(*this);
        RBModules::Scene::Init(*this);
}

mrbcpp::VM& RubyEngine::GetRubyVM()
{
        return m_RubyVM;
}

mrbcpp::Module& RubyEngine::GetBorekModule()
{
        return m_BorekModule;
}

Compiler& RubyEngine::GetCompiler()
{
        return m_Compiler;
}

std::vector<std::string>& RubyEngine::GetClasses()
{
        m_DefinedClasses.clear();

        mrb_state* mrb = m_RubyVM;
        mrb_value scriptable_class = m_BorekModule.get_class("Scriptable");
        RArray* available_classes = mrb_ary_ptr(MRB_FUNCALL(scriptable_class,
                                                            "subclasses"));

        mrb_ssize size = ARY_LEN(available_classes);
        mrb_value* class_names = ARY_PTR(available_classes);

        auto arena = mrb_gc_arena_save(mrb);
        for (int i = 0; i < size; i++) {
                char* cnames = mrb_str_to_cstr(mrb, mrb_obj_as_string(mrb, class_names[i]));
                m_DefinedClasses.emplace_back(cnames);
        }
        mrb_gc_arena_restore(mrb, arena);

        return m_DefinedClasses;
}

void
RubyEngine::Run(const std::string& code)
{
        mrb_state* mrb = m_RubyVM.GetSelf();

        int ai = mrb_gc_arena_save(mrb);
        mrb_load_string(mrb, code.c_str());
        if (mrb->exc) {
                mrb_print_error(mrb);
                mrb_print_backtrace(mrb);
        }
        mrb_gc_arena_restore(mrb, ai);
}

}  // namespace Borek

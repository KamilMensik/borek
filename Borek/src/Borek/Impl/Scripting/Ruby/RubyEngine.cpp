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

RubyEngine::RubyEngine()
{
        m_RubyVM = mrbcpp::VM();
        m_BorekModule = m_RubyVM.define_module("Borek");
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

std::vector<const char*>& RubyEngine::GetClasses()
{
        m_DefinedClasses.clear();

        mrb_state* mrb = m_RubyVM;
        mrb_value scriptable_class = m_BorekModule.get_class("Scriptable");
        RArray* available_classes = mrb_ary_ptr(MRB_FUNCALL(scriptable_class,
                                                            "subclasses"));

        mrb_ssize size = ARY_LEN(available_classes);
        mrb_value* class_names = ARY_PTR(available_classes);

        for (int i = 0; i < size; i++) {
                m_DefinedClasses.emplace_back(mrb_string_cstr(mrb, mrb_obj_as_string(mrb, class_names[i])));
        }

        return m_DefinedClasses;
}




}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/class.h>

#include <mrbcpp.h>
#include <ECS/Component.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBScene.h"
#include "Include/Base/Application.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

MRB_FUNC(GetChild) {
        Node* child = Application::GetScene()->m_RootNode->first_child;
        std::string name = mrb_str_to_cstr(mrb, MRB_ARG1);

        while (child != nullptr) {
                if (child->entity.GetName() == name)
                        break;

                child = child->next;
        }

        if (child) {
                mrb_value val = MRB_NUM(child->entity.GetId());
                return mrb_class_new_instance(mrb, 1, &val, Application::GetRubyEngine().GetBorekModule().get_class("Entity"));
        } else {
                return MRB_NIL;
        }
}

void Scene::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        borek.define_class("Scene")
                .define_class_method("get_child", GetChild,
                                     FuncArgs().Required(1));
}

}  // namespace RBModules
}  // namespace Borek

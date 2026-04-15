// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Base/Entity.h"
#include "Include/Components/ParticleComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBParticleEmmiter.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Enabled)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        return MRB_BOOL(e.GetComponent<ParticleComponent>().enabled);
}

static MRB_FUNC(SetEnabled)
{
        bool arg = mrb_bool(MRB_ARG1);

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        e.GetComponent<ParticleComponent>().enabled = arg;
        return MRB_NIL;
}

static MRB_FUNC(EmmitsPerSecond)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        return MRB_NUM(e.GetComponent<ParticleComponent>().emmits_per_second);
}

static MRB_FUNC(SetEmmitsPerSecond)
{
        int arg = std::max(mrb_as_int(mrb, MRB_ARG1), 0l);
        if (mrb->exc)
                return MRB_NIL;

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        e.GetComponent<ParticleComponent>().emmits_per_second = arg;
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBParticleEmmiter::particle_emmiter_class,
                               NodeType::ParticleEmmiter);
}

NODE_MODULE_METHOD(Enabled);
NODE_MODULE_METHOD(SetEnabled);
NODE_MODULE_METHOD(EmmitsPerSecond);
NODE_MODULE_METHOD(SetEmmitsPerSecond);

void
RBParticleEmmiter::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        particle_emmiter_class = vm.define_class("ParticleEmmiterC", RBNode::node_class)
                .define_method("enabled", Enabled)
                .define_method("enabled=", SetEnabled, FuncArgs().Required(1))
                .define_method("emmits_per_second", EmmitsPerSecond)
                .define_method("emmits_per_second=", SetEmmitsPerSecond, FuncArgs().Required(1));

        particle_emmiter_module = vm.define_module("ParticleEmmiter")
                .include(RBNode::node_module)
                .define_method("enabled", MEnabled)
                .define_method("enabled=", MSetEnabled, FuncArgs().Required(1))
                .define_method("emmits_per_second", MEmmitsPerSecond)
                .define_method("emmits_per_second=", MSetEmmitsPerSecond, FuncArgs().Required(1))
                .define_module_method("new", New);
}

Module RBParticleEmmiter::particle_emmiter_module;
Class RBParticleEmmiter::particle_emmiter_class;

}  // namespace RBModules
}  // namespace vm

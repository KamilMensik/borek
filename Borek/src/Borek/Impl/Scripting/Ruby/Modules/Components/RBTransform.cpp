// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/boxing_word.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTransform.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        mrb_value node = MRB_ARG1;
        std::array<mrb_value, 3> args;

        auto& tran = Entity(mrb_integer(MRB_GET_IV(node, "@entity_id"))).Transform();
        args = { self, MRB_NUM(tran.position.x), MRB_NUM(tran.position.y) };

        MRB_SET_IV(self, "@position",
                   mrb_class_new_instance(mrb, 3, args.data(), RBNDatatypes::vec2_class));

        args = { self, MRB_NUM(tran.scale.x), MRB_NUM(tran.scale.y) };
        MRB_SET_IV(self, "@scale",
                   mrb_class_new_instance(mrb, 3, args.data(), RBNDatatypes::vec2_class));

        MRB_SET_IV(self, "@rotation", MRB_FLOAT(tran.rotation));
        MRB_SET_IV(self, "@node", node);
        
        return self;
}

static MRB_FUNC(Notify)
{
        ChangesExporter::AddTransform(mrb, MRB_GET_IV(self, "@node"));
        return MRB_NIL;
}

void RBTransform::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        transform_class = borek.define_class("Transform")
                .define_method("initialize", Initialize, FuncArgs().Required(1))
                .define_method("notify", Notify);
}

Class RBTransform::transform_class;

}  // namespace RBModules
}  // namespace Borek

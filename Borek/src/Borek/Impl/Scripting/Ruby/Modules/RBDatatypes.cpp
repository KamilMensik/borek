// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>

#include <mrbcpp.h>
#include <ECS/Component.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

/*
 * class Borek::Vec2
 *      attr_accessor :x, :y
 *      def intialize(x: 0, y: 0)
 *              @x = x
 *              @y = y
 *      end
 * end
 * */

MRB_FUNC(Vec2Initialize)
{
        mrb_float x = 0;
        mrb_float y = 0;
        mrb_get_args(mrb, "|ff", &x, &y);

        MRB_SET_IV(self, "@x", MRB_FLOAT(x));
        MRB_SET_IV(self, "@y", MRB_FLOAT(y));

        return self;
}

MRB_FUNC(Vec2Add)
{
        MRB_SET_IV(self, "@x", MRB_FLOAT(mrb_float(MRB_GET_IV(self, "@x")) +
                                         mrb_float(MRB_GET_IV(MRB_ARG1, "@x"))));
        MRB_SET_IV(self, "@y", MRB_FLOAT(mrb_float(MRB_GET_IV(self, "@y")) +
                                         mrb_float(MRB_GET_IV(MRB_ARG1, "@y"))));
        return self;
}

MRB_FUNC(Vec2Subtract)
{
        MRB_SET_IV(self, "@x", MRB_FLOAT(mrb_float(MRB_GET_IV(self, "@x")) -
                                         mrb_float(MRB_GET_IV(MRB_ARG1, "@x"))));
        MRB_SET_IV(self, "@y", MRB_FLOAT(mrb_float(MRB_GET_IV(self, "@y")) -
                                         mrb_float(MRB_GET_IV(MRB_ARG1, "@y"))));
        return self;
}

MRB_FUNC(Vec2ToS)
{
        return MRB_STRING(std::format("Vec2({}, {})",
                                      mrb_float(MRB_GET_IV(self, "@x")),
                                      mrb_float(MRB_GET_IV(self, "@y"))).c_str());
}

void Datatypes::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        borek.define_class("Vec2")
                .define_method("initialize", Vec2Initialize,
                               FuncArgs().Optional(2))
                .define_method("to_s", Vec2ToS)
                .define_attr_accessor<"x">()
                .define_attr_accessor<"y">();
}

}  // namespace RBModules
}  // namespace Borek

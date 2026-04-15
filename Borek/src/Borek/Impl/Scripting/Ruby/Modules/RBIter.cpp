// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mrbcpp.h>
#include <mruby/istruct.h>
#include <mruby/data.h>
#include <mruby/proc.h>
#include <mruby/string.h>

#include "Include/Debug/Log.h"
#include "Include/Scripting/Ruby/Modules/RBIter.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        auto data = MRB_ISTRUCT_VAL(self, RBIter::Data);
        mrb_get_args(mrb, "on", &data->obj, &data->fsym);
        data->index = UINT32_MAX; 
        return self;
}

static MRB_FUNC(Each)
{
        auto data = MRB_ISTRUCT_VAL(self, RBIter::Data);
        mrb_get_args(mrb, "|&", &data->block);

        if (mrb_nil_p(data->block))
                return self;

        return mrb_funcall_with_block(mrb, data->obj, data->fsym, 0, nullptr,
                                      data->block);
}

static MRB_FUNC(WithIndexBlock)
{
        self = mrb_proc_cfunc_env_get(mrb, 0);
        auto data = MRB_ISTRUCT_VAL(self, RBIter::Data);
        mrb_value params[2];

        params[0] = MRB_ARG1;
        params[1] = MRB_NUM(data->index);

        mrb_value res = mrb_yield_argv(mrb, data->block, 2, params);

        data->index++;

        return res;
}

static MRB_FUNC(WithIndex)
{
        mrb_value it = mrb_obj_value(
                mrb_obj_alloc(mrb, MRB_TT_ISTRUCT, RBIter::iter_class));

        mrb_istruct_copy(it, self);
        auto data = MRB_ISTRUCT_VAL(it, RBIter::Data);

        mrb_int index = 0;
        mrb_get_args(mrb, "|i&", &index, &data->block);
        data->index = index;

        if (mrb_nil_p(data->block))
                return self;

        auto blk = mrb_obj_value(
                mrb_proc_new_cfunc_with_env(mrb, WithIndexBlock, 1, &it));

        return mrb_funcall_with_block(mrb, data->obj, data->fsym, 0, nullptr, blk);
}

void
RBIter::Init(class RubyEngine& engine)
{
        auto& vm = engine.GetRubyVM();

        iter_class = vm.define_class("Iterator", MRB_TT_ISTRUCT)
                .define_method("initialize", Initialize, FuncArgs().Required(2))
                .define_method("each", Each, FuncArgs().Block())
                .define_method("with_index", WithIndex, FuncArgs().Optional(1).Block());
}

Class RBIter::iter_class;

}  // namespace RBModules
}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <mrbcpp.h>
#include <mruby/value.h>
#include <mruby/istruct.h>
#include <mruby/boxing_word.h>

#include "Include/Scripting/Ruby/Modules/RBMsgConnection.h"
#include "Include/Core.h"
#include "Include/Base/MsgConnection.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Connect)
{
        mrb_value destination;
        mrb_sym func;
        mrb_get_args(mrb, "on", &destination, &func);

        auto conn = MRB_ISTRUCT_VAL(self, RBMsgConnection::Data);
        mrb_value eid = MRB_GET_IV(MRB_FUNCALL(destination, "node"), "@entity_id");

        conn->connection->entity_id = mrb_integer(eid);
        conn->connection->rbsym = func;

        return MRB_NIL;
}

static MRB_FUNC(Disconnect)
{
        auto conn = MRB_ISTRUCT_VAL(self, RBMsgConnection::Data);
        conn->connection->entity_id = UINT32_MAX;
        conn->connection->rbsym = UINT32_MAX;

        return MRB_NIL;
}

void
RBMsgConnection::Init(class RubyEngine& engine)
{
        auto& vm = engine.GetRubyVM();

        msg_connection_class = vm.define_class("MsgConnection", MRB_TT_ISTRUCT)
                .define_method("connect", Connect, FuncArgs().Required(2))
                .define_method("disconnect", Disconnect);
}

Class RBMsgConnection::msg_connection_class;

}  // namespace RBModules
}  // namespace Borek

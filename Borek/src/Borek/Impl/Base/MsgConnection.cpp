// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Exceptions/RubyException.h"
#include <cstdint>
#include <mruby.h>

#include "Include/Base/MsgConnection.h"
#include "Include/Base/Application.h"
#include "Include/Components/RubyScriptComponent.h"

namespace Borek {

void
MsgConnection::Call(uint64_t args)
{
        if (entity_id == UINT32_MAX || rbsym == UINT32_MAX)
                return;

        Entity e(entity_id);
        if (!e.HasComponent<RubyScriptComponent>())
                return;

        const mrb_value val{args};
        const auto& rsc = Entity{entity_id}.GetComponent<RubyScriptComponent>();
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        if (args == UINT64_MAX) {
                mrb_funcall_id(mrb, { rsc.ruby_instance }, rbsym, 0);
        } else {
                mrb_funcall_id(mrb, { rsc.ruby_instance}, rbsym, 1, val);
        }

        if (mrb->exc) {
                throw RubyException("");
        }
}

bool
MsgConnection::operator ==(const MsgConnection& other) const
{
        return entity_id == other.entity_id && rbsym == other.rbsym;
}

}  // namespace Borek

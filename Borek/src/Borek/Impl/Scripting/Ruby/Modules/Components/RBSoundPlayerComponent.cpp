// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Scripting/Ruby/Modules/RBMsgConnection.h"
#include <mrbcpp.h>
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/istruct.h>

#include "Include/Components/SoundplayerComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSoundPlayerComponent.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

//static Class vec2;
//static Class vec3;
//static Class vec4;


static MRB_FUNC(Initialize)
{
        mrb_value node = MRB_ARG1;
        Entity e(mrb_integer(MRB_GET_IV(node, "@entity_id")));
        auto& sc = e.GetComponent<SoundPlayerComponent>();

        auto on_finished = mrb_class_new_instance(
                mrb, 0, nullptr, RBMsgConnection::msg_connection_class);

        auto onf_data = MRB_ISTRUCT_VAL(on_finished, RBMsgConnection::Data);
        onf_data->connection = sc.on_finished.get();
        MRB_SET_IV(self, "@on_finished", on_finished);
        
        return self;
}

void RBSoundPlayerComponent::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        sound_player_component_class = borek.define_class("SoundPlayerComponent")
                .define_method("initialize", Initialize,
                               FuncArgs().Required(1));
}

Class RBSoundPlayerComponent::sound_player_component_class;

}  // namespace RBModules
}  // namespace Borek

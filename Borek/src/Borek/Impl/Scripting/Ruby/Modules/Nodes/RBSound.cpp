// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSound.h"
#include "Include/Base/Entity.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Base/Application.h"
#include "Include/Debug/Log.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include "Include/Scripting/Ruby/Modules/Components/RBSoundPlayerComponent.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/Asset.h"

#define GET_SOUND_SAFE                                                         \
mrb_value sound = MRB_GET_IV(self, "@_sound");                                 \
if (mrb_nil_p(sound)) {                                                        \
        sound = mrb_class_new_instance(                                        \
                mrb, 1, &self,                                                 \
                RBSoundPlayerComponent::sound_player_component_class);         \
        MRB_SET_IV(self, "@_animated_sprite", sound);                          \
}

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Play)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        e.GetComponent<SoundPlayerComponent>().Play();
        return MRB_NIL;
}

static MRB_FUNC(IsPlaying)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        auto& sp = e.GetComponent<SoundPlayerComponent>();
        return MRB_BOOL(sp.flags.HasFlags(SoundPlayerFlags_Playing));
}

static MRB_FUNC(Stop)
{
        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        e.GetComponent<SoundPlayerComponent>().Stop();
        return MRB_NIL;
}

static MRB_FUNC(SetAsset)
{
        mrb_value arg = MRB_ARG1;
        ASSET_TYPE_ASSERT(arg, sound);

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        Asset<SoundAsset> ass(SCAST<uint32_t>(RCAST<uint64_t>(DATA_PTR(arg))));
        e.GetComponent<SoundPlayerComponent>().Switch(ass);
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBSound::sound_class, NodeType::SoundPlayer);
}

static MRB_FUNC(OnFinished)
{
        GET_SOUND_SAFE;
        return MRB_GET_IV(sound, "@on_finished");
}

NODE_MODULE_METHOD(Play);
NODE_MODULE_METHOD(SetAsset);
NODE_MODULE_METHOD(IsPlaying);
NODE_MODULE_METHOD(Stop);
NODE_MODULE_METHOD(OnFinished);

void
RBSound::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();

        sound_class = vm.define_class("SoundPlayerC", RBNode::node_class)
                .define_method("play", Play)
                .define_method("stop", Stop)
                .define_method("asset=", SetAsset, FuncArgs().Required(1))
                .define_method("playing?", IsPlaying)
                .define_method("on_finished", OnFinished);

        sound_module = vm.define_module("SoundPlayer")
                .include(RBNode::node_module)
                .define_method("play", MPlay)
                .define_method("stop", MStop)
                .define_method("playing?", MIsPlaying)
                .define_method("asset=", MSetAsset, FuncArgs().Required(1))
                .define_method("on_finished", MOnFinished)
                .define_module_method("new", New);
}

Module RBSound::sound_module;
Class RBSound::sound_class;

}  // namespace RBModules
}  // namespace vm

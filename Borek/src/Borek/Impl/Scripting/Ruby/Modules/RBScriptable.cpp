// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/hash.h>
#include <mruby/boxing_word.h>
#include <mrbcpp.h>

#include "Include/Engine/Utils/PathHelpers.h"
#include "Include/Engine/Utils/StringHelpers.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBScriptable.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/TexAsset.h"
#include "Include/Engine/Assets/AnimationAsset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/TilemapAsset.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(OnCreate)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnUpdate)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnDestroy)
{
        return mrb_nil_value();
}

static MRB_FUNC(OnEvent)
{
        return mrb_nil_value();
}

static MRB_FUNC(Export)
{
        mrb_value export_hash;
        if (!MRB_IV_DEFINED(self, "@_export")) {
                export_hash = mrb_hash_new(mrb);
                MRB_SET_IV(self, "@_export", export_hash);
        } else {
                export_hash = MRB_GET_IV(self, "@_export");
        }

        mrb_value type, name;
        mrb_get_args(mrb, "oo", &type, &name);
        mrb_hash_set(mrb, export_hash, type, name);

        return MRB_NIL;
}

static MRB_FUNC(Load)
{
        std::string_view path = mrb_string_cstr(mrb, MRB_ARG1);
        std::string_view extension = path.substr(path.find_last_of('.'));
        const std::filesystem::path abs_path = Utils::Path::FromRelative(path);
        if (!std::filesystem::exists(abs_path))
                return MRB_NIL;

        mrb_value aid;
        switch (Hash(extension)) {
        case Hash(".tex"):
                aid = MRB_NUM(std::move(AssetManager::Get<TexAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::tex_class);
        case Hash(".scr"):
                aid = MRB_NUM(std::move(AssetManager::Get<ScriptAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::script_class);
        case Hash(".sst"):
                aid = MRB_NUM(std::move(AssetManager::Get<SpriteSheetAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::spritesheet_class);
        case Hash(".tmap"):
                aid = MRB_NUM(std::move(AssetManager::Get<TilemapAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::tilemap_class);
        case Hash(".snd"):
                aid = MRB_NUM(std::move(AssetManager::Get<SoundAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::sound_class);
        case Hash(".anim"):
                aid = MRB_NUM(std::move(AssetManager::Get<AnimationAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::animation_class);
        case Hash(".scn"):
                aid = MRB_NUM(std::move(AssetManager::Get<SceneAsset>(path)).Take());
                return mrb_class_new_instance(mrb, 1, &aid, RBAsset::scene_class);
        default:
                mrb_raisef(mrb, E_ARGUMENT_ERROR,
                           "Cannot load %s, because extension %s is not supported!",
                           path.data(), extension.data());

                return MRB_NIL;
        }
}


void Scriptable::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();
        borek.define_class("Scriptable")
                .define_method("on_create", OnCreate)
                .define_method("on_update", OnUpdate)
                .define_method("on_destroy", OnDestroy)
                .define_method("on_event", OnEvent)
                .define_method("load", Load, FuncArgs().Required(1))
                .define_class_method("export", Export, FuncArgs().Required(2));
}

}  // namespace RBModules
}  // namespace Borek

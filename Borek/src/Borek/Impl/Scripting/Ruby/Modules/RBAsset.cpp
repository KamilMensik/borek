// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include <cstdint>
#include <format>

#include <mrbcpp.h>
#include <mruby/string.h>
#include <mruby/hash.h>
#include <mruby/data.h>

#include "Include/Core.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include "Include/Base/Application.h"
#include "Include/Engine/EntityInitializer.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static void
asset_free(mrb_state* mrb, void* asset)
{
        uint32_t aid = RCAST<uint64_t>(asset);
        AssetManager::Decrement(aid);
}

static const mrb_data_type
asset_data_type {
        .struct_name = "Asset",
        .dfree = asset_free,
};

static MRB_FUNC(Initialize)
{
        uint64_t aid = mrb_integer(MRB_ARG1);
        mrb_data_init(self, RCAST<void*>(aid), &asset_data_type);

        return self;
}

static MRB_FUNC(ToS)
{
        const uint32_t aid = RCAST<uint64_t>(DATA_PTR(self));
        const char* class_name = mrb_class_name(mrb, RCAST<RData*>(self.w)->c);
        const char* path = AssetManager::GetPath(aid).c_str();

        return mrb_format(mrb, "<%s: %s>", class_name, path);
}

static MRB_FUNC(Create)
{
        const char* path = mrb_string_cstr(mrb, MRB_GET_IV(self, "@path"));
        Scene& scene = *Application::GetScene();
        Entity e = scene.CreateFromPrefab(path);
        Entity root = scene.GetTree().GetRootEntity();
        scene.GetTree().EntityPrependChild(e, root);

        EntityInitializer::InitializeEntity(e);
        mrb_value node = { e.GetRubyNode() };
        mrb_value parent_node = {root.GetRubyNode()};

        mrb_hash_set(mrb, MRB_GET_IV(parent_node, "@children"),
                     mrb_str_new_cstr(mrb, e.GetName()), node);
        MRB_SET_IV(node, "@parent", parent_node);
        return node;
}

void
RBAsset::Init(class RubyEngine& engine)
{
        auto& borek = engine.GetBorekModule();
        Class asset = borek.define_class("Asset")
                .define_method("to_s", ToS)
                .define_method("initialize", Initialize, FuncArgs().Required(1));

        scene_class = borek.define_class("SceneAsset", asset)
                .define_method("create", Create);

        tex_class = borek.define_class("TexAsset", asset);
        sound_class = borek.define_class("SoundAsset", asset);
        spritesheet_class = borek.define_class("SpriteSheetAsset", asset);
        tilemap_class = borek.define_class("TilemapAsset", asset);
        animation_class = borek.define_class("AnimationAsset", asset);
        script_class = borek.define_class("ScriptAsset", asset);
        font_class = borek.define_class("FontAsset", asset);
}

mrbcpp::Class RBAsset::scene_class;
mrbcpp::Class RBAsset::tex_class;
mrbcpp::Class RBAsset::sound_class;
mrbcpp::Class RBAsset::spritesheet_class;
mrbcpp::Class RBAsset::tilemap_class;
mrbcpp::Class RBAsset::animation_class;
mrbcpp::Class RBAsset::script_class;
mrbcpp::Class RBAsset::font_class;

}  // namespace RBModules
}  // namespace Borek

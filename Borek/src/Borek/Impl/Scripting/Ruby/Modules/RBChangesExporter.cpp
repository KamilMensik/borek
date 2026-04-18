// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "glm/ext/vector_float2.hpp"
#include <mrbcpp.h>
#include <mruby.h>
#include <mruby/hash.h>
#include <mruby/boxing_word.h>
#include <mruby/value.h>
#include <mruby/array.h>

#include "Include/Base/Entity.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"
#include "Include/Base/Symbol.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Utils/GeometryUtils.h"
#include "Include/Base/TransformCache.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static int
upload_transform_fn(mrb_state* mrb, mrb_value eid, mrb_value node, void* _)
{
        Entity e(mrb_integer(eid));
        TransformComponent& t = e.Transform();

        mrb_value transform = MRB_GET_IV(node, "@_transform");
        t.position = *MRB_ISTRUCT_VAL(MRB_GET_IV(transform, "@position"), glm::vec2);
        t.scale = scale_toi(*MRB_ISTRUCT_VAL(MRB_GET_IV(transform, "@scale"), glm::vec2));
        t.rotation = mrb_float(MRB_GET_IV(transform, "@rotation"));

        TransformCache::Invalidate(e);

        return 0;
}

static int
upload_sprite_fn(mrb_state* mrb, mrb_value eid, mrb_value node, void* _)
{
        Entity e(mrb_integer(eid));
        auto& spr = e.GetComponent<SpriteComponent>();

        mrb_value sprite = MRB_GET_IV(node, "@_sprite");
        auto size = MRB_ISTRUCT_VAL(MRB_GET_IV(sprite, "@size"), glm::vec2);

        spr.size_x = uint16_t(size->x);
        spr.size_y = uint16_t(size->y);
        spr.flags.SetFlags(SpriteComponentFlags_FlipX, mrb_bool(MRB_GET_IV(sprite, "@flip_x")));
        spr.flags.SetFlags(SpriteComponentFlags_FlipY, mrb_bool(MRB_GET_IV(sprite, "@flip_y")));

        return 0;
}

static int
upload_animated_sprite_fn(mrb_state* mrb, mrb_value eid, mrb_value node, void* _)
{
        Entity e(mrb_integer(eid));
        auto& spr = e.GetComponent<AnimatedSpriteComponent>();

        mrb_value sprite = MRB_GET_IV(node, "@_animated_sprite");
        auto size = MRB_ISTRUCT_VAL(MRB_GET_IV(sprite, "@size"), glm::vec2);

        spr.size_x = uint16_t(size->x);
        spr.size_y = uint16_t(size->y);

        spr.flags.SetFlags(AnimatedSpriteComponentFlags_FlipX, mrb_bool(MRB_GET_IV(sprite, "@flip_x")));
        spr.flags.SetFlags(AnimatedSpriteComponentFlags_FlipY, mrb_bool(MRB_GET_IV(sprite, "@flip_y")));
        
        Symbol sym = Symbol::FromRB(mrb_symbol(MRB_GET_IV(sprite, "@current_anim")));
        if (!spr.ChangeAnimation(sym))
                BOREK_ENGINE_INFO("Animated Sprite doesnt have animation {}",
                sym.Str());

        return 0;
}

static MRB_FUNC(UploadChanges)
{
        mrb_value hash = MRB_GET_IV(self, "@changed_transforms");
        mrb_hash_foreach(mrb, mrb_hash_ptr(hash), upload_transform_fn, nullptr);
        mrb_hash_clear(mrb, hash);

        hash = MRB_GET_IV(self, "@changed_sprites");
        mrb_hash_foreach(mrb, mrb_hash_ptr(hash), upload_sprite_fn, nullptr);
        mrb_hash_clear(mrb, hash);

        hash = MRB_GET_IV(self, "@changed_animated_sprites");
        mrb_hash_foreach(mrb, mrb_hash_ptr(hash), upload_animated_sprite_fn, nullptr);
        mrb_hash_clear(mrb, hash);

        return MRB_NIL;
}

void
ChangesExporter::Init(RubyEngine &vm)
{
        mrb_state* mrb = vm.GetRubyVM();
        auto& borek = vm.GetBorekModule();
        exporter_class = borek.define_class("ChangesExporter")
                .define_class_iv("@changed_transforms", mrb_hash_new(mrb))
                .define_class_iv("@changed_sprites", mrb_hash_new(mrb))
                .define_class_iv("@changed_animated_sprites", mrb_hash_new(mrb))
                .define_class_method("upload", UploadChanges);
}

void
ChangesExporter::AddTransform(mrb_state* mrb, mrb_value node)
{
        mrb_hash_set(mrb, MRB_GET_IV(exporter_class, "@changed_transforms"),
                     MRB_GET_IV(node, "@entity_id"), node);
}

void
ChangesExporter::AddSprite(mrb_state* mrb, mrb_value node)
{
        mrb_hash_set(mrb, MRB_GET_IV(exporter_class, "@changed_sprites"),
                     MRB_GET_IV(node, "@entity_id"), node);
}

void
ChangesExporter::AddAnimatedSprite(mrb_state* mrb, mrb_value node)
{
        mrb_hash_set(mrb, MRB_GET_IV(exporter_class, "@changed_animated_sprites"),
                     MRB_GET_IV(node, "@entity_id"), node);
}

void
ChangesExporter::Upload(mrb_state* mrb)
{
        UploadChanges(mrb, exporter_class);
}

void
ChangesExporter::Reset(mrb_state* mrb)
{
        mrb_value hash = MRB_GET_IV(exporter_class, "@changed_transforms");
        mrb_hash_clear(mrb, hash);

        hash = MRB_GET_IV(exporter_class, "@changed_sprites");
        mrb_hash_clear(mrb, hash);

        hash = MRB_GET_IV(exporter_class, "@changed_animated_sprites");
        mrb_hash_clear(mrb, hash);
}

mrbcpp::Class ChangesExporter::exporter_class;

}  // namespace RBModules
}  // namespace Borek

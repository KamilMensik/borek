// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include <mrbcpp.h>
#include <mruby/boxing_word.h>
#include <mruby/value.h>
#include <mruby/hash.h>

#include "Include/Engine/EntityInitializer.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBAnimatedSprite.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBDynamicBody.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSound.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSprite.h"
#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/RubyScriptComponent.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Graphics/Camera.h"
#include "Include/Components/ValueComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBArea.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBCamera.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBParticleEmmiter.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBText2D.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBTilemap.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBValue.h"

namespace Borek {

static std::vector<mrb_value> ruby_nodes;

void
EntityInitializer::InitializeEntity(Entity e)
{
        Application::GetScene()->GetTree().TraverseScene(
                e, InitializeBegin, InitializeEnd);
}

void
EntityInitializer::InitializeBegin(Entity e)
{
        switch (e.GetNodeType()) {
        case NodeType::Node:
        case NodeType::Camera:
        case NodeType::Text:
        case NodeType::Sprite:
        case NodeType::ParticleEmmiter:
                break;
        case NodeType::DynamicBody:
        case NodeType::StaticBody:
                InitializeFZXBody(e);
                break;
        case NodeType::Area:
                InitializeFZXArea(e);
                break;
        case NodeType::Tilemap:
                InitializeTilemap(e);
                break;
        case NodeType::SoundPlayer:
                InitializeSoundPlayer(e);
                break;
        case NodeType::AnimatedSprite:
                InitializeAnimatedSprite(e);
                break;
        }
        
        if (e.HasComponent<ValueComponent>()) {
                InitializeValues(e);
        }

        InitializeRubyNode(e);
}

void
EntityInitializer::InitializeEnd(Entity e)
{
        ruby_nodes.pop_back();
}

void
EntityInitializer::InitializeFZXBody(Entity e)
{
        auto& body = e.GetComponent<BodyComponent>();

        auto& tc = e.GlobalTransform();

        Application::GetScene()->m_PhysicsWorld.Add(
                e, body, tc.position, scale_tof(tc.scale));
}

void
EntityInitializer::InitializeFZXArea(Entity e)
{
        auto& area = e.GetComponent<AreaComponent>();

        auto& tc = e.GlobalTransform();
        Application::GetScene()->m_PhysicsWorld.Add(
                e, area, tc.position, scale_tof(tc.scale));
}

void
EntityInitializer::InitializeTilemap(Entity e)
{
        TilemapComponent& tc = e.GetComponent<TilemapComponent>();
        if (!tc.tilemap.IsValid())
                return;

        auto& transform = e.GlobalTransform();
        SpriteSheetAsset& spritesheet = tc.tilemap->sprite_sheet.Convert();
        const glm::vec2& scale = scale_tof(transform.scale);
        const glm::vec2 real_scale(spritesheet.step_x * scale.x,
                                   spritesheet.step_y * scale.y);

        FZX::FACD& fzxw = Application::GetScene()->m_PhysicsWorld;
        FZX::Body body;
        body.body_type = FZX::BodyType::Static;
        body.collider_type = FZX::ColliderType::Tilemap;

        for (auto it : tc.items) {
                const auto& pos = it.first;
                const uint16_t index = it.second;

                if (!tc.tilemap->tile_coliders[index].Exists())
                        continue;

                const glm::vec2 real_pos(pos.second * tc.step_x * scale.x, pos.first * tc.step_y * scale.y);
                body.tc.row = pos.first;
                body.tc.col = pos.second;
                tc.existing_colliders[{pos.first, pos.second}] = fzxw.Add(e, body, real_pos, real_scale);
        }
}

void
EntityInitializer::InitializeSoundPlayer(Entity e)
{
        auto& sp = e.GetComponent<SoundPlayerComponent>();
        if (sp.asset.IsValid() && sp.flags.HasFlags(SoundPlayerFlags_Autoplay)) {
                sp.Play();
        }
}

void
EntityInitializer::InitializeRubyNode(Entity e)
{
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        mrb_value eid = mrb_fixnum_value(e.GetId());

        RClass* cls;
        switch (e.GetNodeType()) {
        case NodeType::StaticBody:
                cls = RBModules::RBNode::node_class;
                break;
        case NodeType::DynamicBody:
                cls = RBModules::RBDynamicBody::dynamic_body_class;
                break;
        case NodeType::Sprite:
                cls = RBModules::RBSprite::sprite_class;
                break;
        case NodeType::SoundPlayer:
                cls = RBModules::RBSound::sound_class;
                break;
        case NodeType::AnimatedSprite:
                cls = RBModules::RBAnimatedSprite::sprite_class;
                break;
        case NodeType::Text:
                cls = RBModules::RBText2D::text2d_class;
                break;
        case NodeType::ParticleEmmiter:
                cls = RBModules::RBParticleEmmiter::particle_emmiter_class;
                break;
        case NodeType::Tilemap:
                cls = RBModules::RBTilemap::tilemap_class;
                break;
        case NodeType::Camera:
                cls = RBModules::RBCamera::camera_class;
                break;
        case NodeType::Node:
                cls = RBModules::RBNode::node_class;
                break;
        case NodeType::Area:
                cls = RBModules::RBArea::area_class;
                break;
        }

        mrb_value node = mrb_class_new_instance(mrb, 1, &eid, cls);
        mrb_gc_register(mrb, node);
        if (e.HasComponent<ValueComponent>()) {
                RBModules::RBValue::InitAccessors(node);
        }

        if (e.HasComponent<RubyScriptComponent>()) {
                auto& rsc = e.GetComponent<RubyScriptComponent>();
                rsc.Initialize(e);
                MRB_SET_IV({rsc.ruby_instance}, "@_node", node);
        }

        if (!ruby_nodes.empty()) {
                mrb_hash_set(mrb, MRB_GET_IV(ruby_nodes.back(), "@children"),
                             mrb_str_new_cstr(mrb, e.GetName()), node);
                MRB_SET_IV(node, "@parent", ruby_nodes.back());
        }

        Application::GetScene()->GetTree().SetEntityRubyNode(e, node.w);
        ruby_nodes.emplace_back(node);
}

void
EntityInitializer::InitializeAnimatedSprite(Entity e)
{
        auto& spr = e.GetComponent<AnimatedSpriteComponent>();
        spr.flags.SetFlags(AnimatedSpriteComponentFlags_Playing, true);
        spr.current_frame = spr.anim->animation_by_name[spr.current_animation] + 1;
}

void
EntityInitializer::InitializeValues(Entity e)
{
        auto& values = e.GetComponent<ValueComponent>();
        
        for (auto& val : values) {
                switch (val.type) {
                case ValueType_Asset:
                        val.asset.asset_id = AssetManager::GetRaw(val.asset.path.Str());
                        break;
                case ValueType_Node:
                        val.node.entity_id = e.FindEntityByPath(val.node.path.Str());
                        break;
                default:
                        break;
                }
        }
}

}  // namespace Borek

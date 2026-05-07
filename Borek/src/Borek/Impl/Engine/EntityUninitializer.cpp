// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/ValueComponent.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/EntityUninitializer.h"

namespace Borek {

void
EntityUninitializer::UninitializeEntity(Entity e)
{
        Application::GetScene()->GetTree().TraverseScene(e, Uninitialize);
}

void
EntityUninitializer::Uninitialize(Entity e)
{
        switch (e.GetNodeType()) {
        case NodeType::Node:
        case NodeType::Camera:
        case NodeType::Text:
        case NodeType::Sprite:
        case NodeType::AnimatedSprite:
        case NodeType::ParticleEmmiter:
                break;
        case NodeType::DynamicBody:
        case NodeType::StaticBody:
                UninitializeFZXBody(e);
                break;
        case NodeType::Area:
                UninitializeFZXArea(e);
                break;
        case NodeType::Tilemap:
                UninitializeTilemap(e);
                break;
        case NodeType::SoundPlayer:
                UninitializeSoundPlayer(e);
                break;
        }

        UninitializeRubyNode(e);
        if (e.HasComponent<ValueComponent>()) {
                UninitializeValues(e);
        }
}

void
EntityUninitializer::UninitializeFZXBody(Entity e)
{
        auto& body = e.GetComponent<BodyComponent>();

        Application::GetScene()->m_PhysicsWorld.Remove(e, body);
}

void
EntityUninitializer::UninitializeFZXArea(Entity e)
{
        auto& area = e.GetComponent<AreaComponent>();

        Application::GetScene()->m_PhysicsWorld.Remove(e, area);
}

void
EntityUninitializer::UninitializeTilemap(Entity e)
{
        TilemapComponent& tc = e.GetComponent<TilemapComponent>();
        if (tc.existing_colliders.empty())
                return;

        FZX::Body body;
        body.collider_type = FZX::ColliderType::Tilemap;
        body.body_type = FZX::BodyType::Static;
        Application::GetScene()->m_PhysicsWorld.Remove(e, body);
}

void
EntityUninitializer::UninitializeSoundPlayer(Entity e)
{
        auto& sp = e.GetComponent<SoundPlayerComponent>();
        sp.Stop();
}

void
EntityUninitializer::UninitializeRubyNode(Entity e)
{
        mrb_gc_unregister(Application::GetRubyEngine().GetRubyVM(),
                          { e.GetRubyNode() });
}

void
EntityUninitializer::UninitializeValues(Entity e)
{
        auto& values = e.GetComponent<ValueComponent>();
        
        for (auto& val : values) {
                switch (val.type) {
                case ValueType_Asset:
                        if (val.asset.asset_id == UINT32_MAX)
                                return;

                        AssetManager::Decrement(val.asset.asset_id);
                        break;
                default:
                        break;
                }
        }

}

}  // namespace Borek


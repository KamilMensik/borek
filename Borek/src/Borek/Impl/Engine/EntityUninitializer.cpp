// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Graphics/Camera.h"

#include "Include/Engine/EntityUninitializer.h"
#include "Include/Components/TransformComponent.h"

namespace Borek {

void
EntityUninitializer::UninitializeBegin(Entity e)
{
        s_GlobalTransform += e.GetComponent<TransformComponent>();

        switch (e.GetNodeType()) {
        case NodeType::Node:
        case NodeType::Camera:
        case NodeType::Text:
        case NodeType::Sprite:
                break;
        case NodeType::DynamicBody:
        case NodeType::StaticBody:
        case NodeType::Area:
                UninitializeFZXBody(e);
                break;
        case NodeType::Tilemap:
                UninitializeTilemap(e);
                break;
        case NodeType::SoundPlayer:
                UninitializeSoundPlayer(e);
                break;
        }
}

void
EntityUninitializer::UninitializeEnd(Entity e)
{
        s_GlobalTransform -= e.GetComponent<TransformComponent>();
}

void
EntityUninitializer::UninitializeFZXBody(Entity e)
{
        auto& body = e.GetComponent<FZX::BodyComponent>();

        //body.Update(s_GlobalTransform.position, s_GlobalTransform.scale);
        //Application::GetScene()->m_PhysicsWorld.Remove(e, body);
}

void
EntityUninitializer::UninitializeTilemap(Entity e)
{
        //TilemapComponent& tc = e.GetComponent<TilemapComponent>();
        //if (!tc.tilemap.IsValid())
        //        return;

        //SpriteSheetAsset& spritesheet = tc.tilemap->sprite_sheet.Convert();
        //const glm::vec2& scale = s_GlobalTransform.scale;

        //for (auto it : tc.items) {
        //        const auto& pos = it.first;
        //        const uint16_t index = it.second;

        //        if (!tc.tilemap->tile_coliders[index].Exists())
        //                continue;

        //        const glm::vec2 real_pos(pos.second * tc.step_x * scale.x, pos.first * tc.step_y * scale.y);
        //        FZX::Body body;
        //        body.Update(real_pos, { spritesheet.step_x * scale.x,
        //                                spritesheet.step_y * scale.y });
        //        Application::GetScene()->m_PhysicsWorld.Add(e, body);
        //}
}

void
EntityUninitializer::UninitializeSoundPlayer(Entity e)
{
        auto& sp = e.GetComponent<SoundPlayerComponent>();
        sp.Stop();
}

TransformComponent EntityUninitializer::s_GlobalTransform = TransformComponent();
}  // namespace Borek


// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include "Include/Drawing/Scene.h"
#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/Text2DComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Components/ZIndexComponent.h"
#include "Include/Base/Query.h"
#include "Include/Drawing/Font.h"
#include "Include/Drawing/Quad.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Components/ParticleComponent.h"
#include "Include/Engine/ParticleSystem.h"

namespace Borek {
namespace Drawing {

static const void
load_transform(std::unordered_map<uint32_t, TransformComponent>& map, Entity e)
{
        if (auto it = map.find(e.GetId()); it != map.end())
                return;

        Entity parent = e.GetParent();
        if (parent.IsNil()) {
                map[e.GetId()] = e.Transform();
                return;
        }

        load_transform(map, parent);
        map[e.GetId()] = e.Transform();
        map[e.GetId()] += map[parent.GetId()];
}

void
Scene::Draw()
{
        std::unordered_map<uint32_t, TransformComponent> transform_map;
        
        auto query_1 = Query<IDComponent, SpriteComponent, ZIndexComponent>();
        auto query_2 = Query<IDComponent, TilemapComponent, ZIndexComponent>();
        auto query_3 = Query<IDComponent, AnimatedSpriteComponent, ZIndexComponent>();
        auto query_4 = Query<IDComponent, Text2DComponent, ZIndexComponent>();
        auto query_5 = Query<IDComponent, ParticleComponent, ZIndexComponent>();

        for (auto& [id, sprite, zindex] : query_1) {
                Entity cur(id->ecs_id);
                load_transform(transform_map, cur);
                Quad::Draw(transform_map[id->ecs_id], *sprite, zindex->value);
        }

        for (auto& [id, tilemap, zindex] : query_2) {
                if (!tilemap->tilemap.IsValid())
                        continue;

                Entity cur(id->ecs_id);
                load_transform(transform_map, cur);
                const glm::vec2& scale = transform_map[cur.m_Id].scale;

                SpriteSheetAsset& spritesheet = tilemap->tilemap->sprite_sheet.Convert();

                for (auto it : tilemap->items) {
                        const auto& pos = it.first;

                        Drawing::Quad::Draw({ pos.second * tilemap->step_x * scale.x,
                                              pos.first * tilemap->step_y * scale.y },
                                            { spritesheet.step_x * scale.x * 1.001f,
                                              spritesheet.step_y * scale.y * 1.001f },
                                            spritesheet.texture,
                                            Colors::WHITE, zindex->value,
                                            spritesheet.SubTextureCords(it.second));
                }
        }

        for (auto& [id, sprite, zindex] : query_3) {
                Entity cur(id->ecs_id);
                load_transform(transform_map, cur);
                Quad::Draw(transform_map[id->ecs_id], *sprite, zindex->value);
                sprite->Step(Application::GetTime());
        }

        for (auto& [id, txt, zindex] : query_4) {
                if (!txt->font.IsValid())
                        continue;

                Entity cur(id->ecs_id);
                load_transform(transform_map, cur);
                Font::Draw(transform_map[id->ecs_id], *txt, zindex->value);
        }

        for (auto& [id, particle, zindex] : query_5) {
                if (!particle->enabled || particle->emmits_per_second == 0)
                        continue;

                Entity cur(id->ecs_id);
                load_transform(transform_map, cur);

                particle->delta_cache += Application::GetDelta();
                particle->delta_cache *= particle->emmits_per_second;

                float intergral;
                particle->delta_cache = std::modf(particle->delta_cache, &intergral);

                for (unsigned i = intergral; i > 0; i--) {
                        ParticleSystem::Emit(*particle,
                                             transform_map[id->ecs_id].position,
                                             zindex->value);
                }

                particle->delta_cache /= particle->emmits_per_second;
        }
}

}  // namespace Drawing
}  // namespace Borek

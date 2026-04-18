// Copyright 2024-2025 <kamilekmensik@gmail.com>

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

void
Scene::Draw()
{
        auto query_1 = Query<IDComponent, SpriteComponent, ZIndexComponent>();
        auto query_2 = Query<IDComponent, TilemapComponent, ZIndexComponent>();
        auto query_3 = Query<IDComponent, AnimatedSpriteComponent, ZIndexComponent>();
        auto query_4 = Query<IDComponent, Text2DComponent, ZIndexComponent>();
        auto query_5 = Query<IDComponent, ParticleComponent, ZIndexComponent>();

        for (auto& [id, sprite, zindex] : query_1) {
                Entity cur(id->ecs_id);
                Quad::Draw(cur.GlobalTransform(), *sprite, zindex->value);
        }

        for (auto& [id, tilemap, zindex] : query_2) {
                if (!tilemap->tilemap.IsValid())
                        continue;

                Entity cur(id->ecs_id);
                auto& tran = cur.GlobalTransform();
                const glm::vec2& scale = scale_tof(tran.scale);

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
                auto& tran = Entity(id->ecs_id).GlobalTransform();
                Quad::Draw(tran, *sprite, zindex->value);
                sprite->Step(Application::GetTime());
        }

        for (auto& [id, txt, zindex] : query_4) {
                if (!txt->font.IsValid())
                        continue;

                auto& tran = Entity(id->ecs_id).GlobalTransform();
                Font::Draw(tran, *txt, zindex->value);
        }

        for (auto& [id, particle, zindex] : query_5) {
                if (!particle->enabled || particle->emmits_per_second == 0)
                        continue;

                auto& tran = Entity(id->ecs_id).GlobalTransform();

                particle->delta_cache += Application::GetDelta();
                particle->delta_cache *= particle->emmits_per_second;

                float intergral;
                particle->delta_cache = std::modf(particle->delta_cache, &intergral);

                for (unsigned i = intergral; i > 0; i--) {
                        ParticleSystem::Emit(*particle,
                                             tran.position,
                                             zindex->value);
                }

                particle->delta_cache /= particle->emmits_per_second;
        }
}

}  // namespace Drawing
}  // namespace Borek

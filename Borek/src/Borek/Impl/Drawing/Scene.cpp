// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Drawing/Scene.h"
#include "ECS/Query.h"
#include "Include/Base/Entity.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Components/ZIndexComponent.h"
#include "Include/Debug/Log.h"
#include "Include/Drawing/Quad.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include <cstdint>

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
        
        auto query_1 = ECS::Query<IDComponent, SpriteComponent, ZIndexComponent>();
        auto query_2 = ECS::Query<IDComponent, TilemapComponent, ZIndexComponent>();

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
}

}  // namespace Drawing
}  // namespace Borek

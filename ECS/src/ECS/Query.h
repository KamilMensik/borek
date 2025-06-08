// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <vector>

#include "ECS/View.h"

namespace ECS {

using ComponentId = uint32_t;

/*
 *      Listener query.
 *      Actively checks if new archetypes with given components are created.
 *
 *      # Adds all existing archetypes
 *      ECS::QueryL render_query({ transform, sprite });
 *      render_query.each([](ECS::View& view) {
 *              RenderSprite(view.get<Transform>(), view.get<Sprite>());
 *      })
 * */
class QueryL {
friend class World;
public:
        void Each(std::function<void(View&)> body);

private:
        QueryL(const std::initializer_list<ComponentId>& components,
               class World&);
        void TryAdd(struct Archetype& archetype);
        std::vector<ComponentId> m_Type;
        std::vector<uint32_t> m_Archetypes;
        class World* m_World;
};

class Query {
public:
        Query() {}
        Query(uint32_t query_id, class World* world);
        void each(std::function<void(View&)> body);

private:
        class World* m_World;
        uint32_t m_QueryId;
};

}  // namespace ECS

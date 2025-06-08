// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Query.h"
#include "ECS/View.h"
#include "ECS/Archetype.h"
#include "ECS/World.h"

namespace ECS {

QueryL::QueryL(const std::initializer_list<ComponentId>& components,
               World& world) : m_Type(components)
{
        m_World = &world;
        for (auto& archetype : world.m_RegisteredArchetypes)
                TryAdd(*archetype);
}

void QueryL::TryAdd(struct Archetype& archetype)
{
        for (auto& component : m_Type) {
                if (!archetype.type.contains(component))
                        return;
        }

        m_Archetypes.emplace_back(archetype.id);
}

void QueryL::Each(std::function<void(View&)> body)
{
        View v(m_Type.size());
        std::vector<uint32_t> component_indexes(m_Type.size());
        for (auto archetype_id : m_Archetypes) {
                for (int i = 0; auto component : m_Type) {
                        component_indexes[i] =
                                m_World->GetCColumn(component, archetype_id);
                        v.c_index[component] = i;
                        i++;
                }

                Archetype& archetype = *m_World->m_RegisteredArchetypes[archetype_id];
                for (unsigned i = 0; i < archetype.entities.size(); i++) {
                        v.current_entity_id = archetype.entities[i];
                        for (int j = 0; auto cid : component_indexes) {
                                v.data[j] = archetype.components[cid][i];
                                j++;
                        }

                        body(v);
                }
        }
}

Query::Query(uint32_t query_id, World* world)
       : m_World(world), m_QueryId(query_id) {}

void Query::each(std::function<void(View&)> body)
{
        m_World->m_LQueries[m_QueryId].Each(body);
}


}  // namespace ECS

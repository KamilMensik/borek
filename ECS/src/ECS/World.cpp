// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Query.h"
#include <iostream>
#include "World.h"

namespace ECS {

World::World()
{
}

World::~World()
{
        for (Id entity : m_WorldEntities) {
                const WData& data = s_WorldData[entity];
                if (!data.Exists())
                        continue;

                s_Reusable.emplace_back(entity);
                Archetype(data.archetype_id).RemoveEntity(data.archetype_row);
        }
}

Id World::entity()
{
        EntityId entity = GetId();
        s_WorldData[entity].archetype_id = 0;
        s_WorldData[entity].archetype_row = 0;
        return entity;
}

Id World::entity(Archetype archetype)
{
        EntityId e = GetId();
        archetype.AddEntity(e);
        m_WorldEntities.insert(e);
        return e;
}
void World::remove(EntityId id)
{
        s_Reusable.emplace_back(id);
        m_WorldEntities.erase(id);
        Archetype(s_WorldData[id].archetype_id)
                .RemoveEntity(s_WorldData[id].archetype_row);
        s_WorldData[id].archetype_id = UINT32_MAX;
        s_WorldData[id].archetype_row = 0;
}

void World::set_components(Id entity, const ArchetypeType& components)
{
        set_components(entity, ArchetypeInternal::Get(components));
}
void World::set_components(EntityId entity, Archetype archetype)
{
        if (auto aid = s_WorldData[entity].archetype_id; aid != UINT32_MAX) {
                Archetype(aid).MoveEntity(entity, archetype);
                return;
        }
}

void World::add_component(EntityId entity, ComponentId component)
{
        Archetype old_a(s_WorldData[entity].archetype_id);
        Archetype edge(old_a.GetEdge(component, true));
        old_a.MoveEntity(entity, edge);
}

void World::remove_component(EntityId entity, ComponentId component)
{
        Archetype old_a(s_WorldData[entity].archetype_id);
        Archetype edge(old_a.GetEdge(component, false));
        s_ComponentData[component]
                .destructor(old_a.GetComponents(component)[s_WorldData[entity].archetype_row]);
        old_a.MoveEntity(entity, edge);
}

bool World::has_component(EntityId entity, ComponentId component)
{
        Archetype archetype(s_WorldData[entity].archetype_id);
        return archetype.HasComponent(component);
}

void* World::get_component(EntityId entity, ComponentId component)
{
        Archetype archetype(s_WorldData[entity].archetype_id);
        uint32_t row_id = s_WorldData[entity].archetype_row;

        return archetype[component][row_id];
}

Id World::GetId()
{
        if (s_Reusable.size() > 0) {
                Id id = s_Reusable.back();
                s_Reusable.pop_back();
                return id;
        }

        s_WorldData.emplace_back(WData());
        return s_CurrentId++;
}

std::vector<WData> World::s_WorldData;
std::vector<Id> World::s_Reusable;
uint32_t World::s_CurrentId = 0;

}  // namespace ECS

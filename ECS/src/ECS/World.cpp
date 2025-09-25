// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Query.h"
#include "World.h"

namespace ECS {

World::World()
{
        m_CurrentId = 0;
}

World::~World()
{
        ArchetypeInternal::Reset();
}

Id World::entity()
{
        EntityId entity = GetId();
        m_WorldData[entity].archetype_id = 0;
        m_WorldData[entity].archetype_row = 0;
        return entity;
}

Id World::entity(Archetype archetype)
{
        EntityId e = GetId();
        archetype.AddEntity(e, *this);
        return e;
}
void World::remove(EntityId id)
{
        m_Reusable.emplace_back(id);
        Archetype(m_WorldData[id].archetype_id)
                .RemoveEntity(id, m_WorldData[id].archetype_row, *this);
        m_WorldData[id].archetype_id = 0;
        m_WorldData[id].archetype_row = 0;
}

void World::set_components(Id entity, const ArchetypeType& components)
{
        set_components(entity, ArchetypeInternal::Get(components));
}
void World::set_components(EntityId entity, Archetype archetype)
{
        if (auto aid = m_WorldData[entity].archetype_id; aid != UINT32_MAX) {
                Archetype(aid).MoveEntity(entity, archetype, *this);
                return;
        }
}

void World::add_component(EntityId entity, ComponentId component)
{
        Archetype old_a(m_WorldData[entity].archetype_id);
        Archetype edge(old_a.GetEdge(component, true));
        old_a.MoveEntity(entity, edge, *this);
}

void World::remove_component(EntityId entity, ComponentId component)
{
        Archetype old_a(m_WorldData[entity].archetype_id);
        Archetype edge(old_a.GetEdge(component, false));
        s_ComponentData[component]
                .destructor(old_a.GetComponents(component)[m_WorldData[entity].archetype_row]);
        old_a.MoveEntity(entity, edge, *this);
}

bool World::has_component(EntityId entity, ComponentId component)
{
        Archetype archetype(m_WorldData[entity].archetype_id);
        return archetype.HasComponent(component);
}

void* World::get_component(EntityId entity, ComponentId component)
{
        Archetype archetype(m_WorldData[entity].archetype_id);
        uint32_t row_id = m_WorldData[entity].archetype_row;

        return archetype[component][row_id];
}

Id World::GetId()
{
        if (m_Reusable.size() > 0) {
                Id id = m_Reusable.back();
                m_Reusable.pop_back();
                return id;
        }

        m_WorldData.emplace_back(WData());
        return m_CurrentId++;
}

}  // namespace ECS

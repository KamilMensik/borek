// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ECS/Query.h"
#include <iostream>
#include <ostream>

#include "World.h"

namespace ECS {

World::World()
{
        m_CurrentId = 0;
        m_RegisteredArchetypes.emplace_back(std::make_shared<Archetype>());
        m_Archetypes[{}] = 0;
}

Id World::entity()
{
        EntityId entity = GetId();
        m_WorldData[entity].archetype_id = 0;
        m_WorldData[entity].archetype_row = 0;
        return entity;
}

Id World::entity(const __AType_IList& components)
{
        return entity(get_archetype(components));
}
Id World::entity(ArchetypeId archetype)
{
        EntityId e = GetId();
        Archetype& a = *m_RegisteredArchetypes[archetype];
        m_WorldData[e].archetype_id = a.id;
        m_WorldData[e].archetype_row = a.add_entity(e);
        return e;
}
void World::remove(EntityId id)
{
        m_Reusable.emplace_back(id);
        m_RegisteredArchetypes[m_WorldData[id].archetype_id]
                ->remove_entity(id, m_WorldData[id].archetype_row, *this);
        m_WorldData[id].archetype_id = 0;
        m_WorldData[id].archetype_row = 0;
}

void World::set_components(Id entity, const __AType_IList& components)
{
        set_components(entity, get_archetype(components));
}
void World::set_components(EntityId entity, ArchetypeId archetype)
{
        Archetype& a = *m_RegisteredArchetypes[archetype];
        if (auto aid = m_WorldData[entity].archetype_id; aid != UINT32_MAX) {
                m_RegisteredArchetypes[aid]->move_entity(entity, a, *this);
                return;
        }

        m_WorldData[entity].archetype_id = archetype;
        m_WorldData[entity].archetype_row = a.add_entity(entity);
}
void World::add_component(EntityId entity, ComponentId component)
{
        auto old_a = m_RegisteredArchetypes[m_WorldData[entity].archetype_id];
        auto edge = m_RegisteredArchetypes[old_a->get_edge(component, *this, true)];
        old_a->move_entity(entity, *edge, *this);
}
void World::remove_component(EntityId entity, ComponentId component)
{
        auto old_a = m_RegisteredArchetypes[m_WorldData[entity].archetype_id];
        auto edge = m_RegisteredArchetypes[old_a->get_edge(component, *this, false)];

        s_ComponentData[component]
                .destructor(old_a->components[GetCColumn(component, old_a->id)][m_WorldData[entity].archetype_row]);
        
        old_a->move_entity(entity, *edge, *this);
}

bool World::has_component(EntityId entity, ComponentId component)
{
        uint32_t archetype_id = m_WorldData[entity].archetype_id;
        return m_ComponentColumn.find(GetCAId(component, archetype_id)) !=
                m_ComponentColumn.end();
}
void* World::get_component(EntityId entity, ComponentId component)
{
        uint32_t archetype_id = m_WorldData[entity].archetype_id;
        uint32_t row_id = m_WorldData[entity].archetype_row;

        auto res = m_ComponentColumn.find(GetCAId(component, archetype_id));
        if (res == m_ComponentColumn.end()) {
                std::cerr << "Entity doesnt have this component\n";
                return nullptr;
        }

        return m_RegisteredArchetypes[archetype_id]->components[res->second][row_id];
}

ArchetypeId World::get_archetype(const std::initializer_list<Id>& type)
{
        auto tryfind = m_Archetypes.find(type);
        if (tryfind != m_Archetypes.end()) {
                return tryfind->second;
        }

        auto a = std::make_shared<Archetype>(type);
        m_RegisteredArchetypes.emplace_back(a);
        a->init(m_RegisteredArchetypes.size() - 1, *this);
        TryAddArchetypeToQueries(*a);
        m_Archetypes[a->type] = a->id;
        return a->id;
}

ArchetypeId World::get_archetype(ArchetypeType& type)
{
        auto tryfind = m_Archetypes.find(type);
        if (tryfind != m_Archetypes.end()) {
                return tryfind->second;
        }

        auto archetype = std::make_shared<Archetype>(type);
        m_RegisteredArchetypes.emplace_back(archetype);
        archetype->init(m_RegisteredArchetypes.size() - 1, *this);
        TryAddArchetypeToQueries(*archetype);
        m_Archetypes[archetype->type] = archetype->id;
        return archetype->id;
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
uint32_t World::GetCColumn(uint32_t component_id, uint32_t archetype_id)
{
        return m_ComponentColumn[GetCAId(component_id, archetype_id)];
}

void World::TryAddArchetypeToQueries(const Archetype& a)
{
        QueryInternal::SetWorld(this);
        for (auto query : QueryInternal::GetDefinedQueries())
                query.TryAdd(a);
}

}  // namespace ECS

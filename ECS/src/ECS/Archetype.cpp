// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ECS/Query.h"
#include "ECS/World.h"
#include <cstring>

#include "Archetype.h"
#include "Component.h"

namespace ECS {

using ArchetypeType = std::unordered_set<ComponentId>;

Id
ArchetypeInternal::Get(const ArchetypeType& type)
{
        auto tryfind = s_ArchetypesByType.find(type);
        if (tryfind != s_ArchetypesByType.end()) {
                return tryfind->second;
        }

        uint32_t id = s_CreatedArchetypes.size();
        s_CreatedArchetypes.emplace_back(type);
        s_CreatedArchetypes[id].Init(id);
        return id;
}

ArchetypeInternal::ArchetypeInternal(const ArchetypeType& type) : m_Type(type) {}

void
ArchetypeInternal::Init(uint32_t id)
{
        m_Id = id;
        m_MaxSize = 0;
        s_ArchetypesByType[m_Type] = id;

        for (int i = 0; ComponentId c : m_Type) {
                ComponentData& cdata = s_ComponentData[c];

                m_Components.emplace_back(cdata.size, cdata.alignment,
                                          cdata.constructor, cdata.destructor);
                s_ComponentColumn[GetCAId(c, id)] = i;
                i++;
        }

        QueryInternal::TryAddArchetypeToQueries(Archetype(m_Id));
}

Id
ArchetypeInternal::GetEdge(ComponentId c, bool is_adding)
{
        if (auto edge = m_Edges.find(c); edge != m_Edges.end()) {
                return edge->second;
        }

        ArchetypeType test = m_Type;
        if (is_adding)
                test.emplace(c);
        else
                test.erase(c);

        int keep_id = m_Id;

        // ArchetypeInternal::Get may invalidate this object
        Id find = ArchetypeInternal::Get(test);

        s_CreatedArchetypes[find].m_Edges[c] = m_Id;
        s_CreatedArchetypes[keep_id].m_Edges[c] = find;

        return find;
}

uint32_t
ArchetypeInternal::AddEntity(EntityId e, World& world, bool init_values)
{
        if (m_Entities.size() >= m_MaxSize)
                ChangeSize((m_MaxSize << 2) + 5);

        uint32_t row = m_Entities.size();
        m_Entities.emplace_back(e);

        world.m_WorldData[e].archetype_row = row;
        world.m_WorldData[e].archetype_id = m_Id;

        if (!init_values)
                return row;

        for (auto& column : m_Components) {
                column.constructor(column[row]);
        }

        return row;
}

uint32_t
ArchetypeInternal::MoveEntity(EntityId e, ArchetypeInternal& dest,
                              World& world)
{
        uint32_t from_row = world.m_WorldData[e].archetype_row;
        uint32_t dest_row = dest.AddEntity(e, world, false);

        for (ComponentId c : dest.m_Type) {
                auto& to = dest.m_Components[s_ComponentColumn[GetCAId(c, dest.m_Id)]];
                auto from_col = s_ComponentColumn.find(GetCAId(c, m_Id));
                if (from_col == s_ComponentColumn.end()) {
                        to.constructor(to[dest_row]);
                } else {
                        std::memcpy(to[dest_row],
                                    m_Components[from_col->second][from_row],
                                    to.element_size);
                }
        }

        RemoveEntity(e, from_row, world, false);
        return dest_row;
}

void ArchetypeInternal::RemoveEntity(EntityId e, uint32_t row, World& world,
                                     bool destruct_values)
{
        if (m_Entities.size() == 0)
                return;

        if (destruct_values) {
                for (auto& c : m_Components)
                        c.destructor(c[row]);
        }

        if (m_Entities.size() <= 1) {
                m_Entities.pop_back();
                return;
        }

        if (row != (m_Entities.size() - 1)) {
                uint32_t replaceworldith = m_Entities.size() - 1;
                for (auto& column : m_Components) {
                        std::memcpy(column[row], column[replaceworldith],
                                    column.element_size);
                }

                m_Entities[row] = m_Entities.back();
                world.m_WorldData[m_Entities.back()].archetype_row = row;
        }

        m_Entities.pop_back();

        if (m_MaxSize / m_Entities.size() >= 8)
                ChangeSize((m_MaxSize - 5) >> 2);
}

void ArchetypeInternal::ChangeSize(uint32_t new_size)
{
        uint8_t *data_to_free = nullptr;
        if (m_MaxSize != 0 && m_Components.size() > 0)
                data_to_free = reinterpret_cast<uint8_t*>(m_Components.front().data);

        uint32_t component_sizes = 0;
        for (auto& component : m_Components)
                component_sizes += component.element_size;
        uint8_t* data = new uint8_t[component_sizes * new_size];

        for (uint32_t offset = 0; auto& component : m_Components) {
                uint8_t *new_cdata = &data[offset];
                std::memcpy(new_cdata, component.data,
                            component.element_size * m_MaxSize);
                component.data = new_cdata;

                offset += component.element_size * new_size;
        }

        m_MaxSize = new_size;

        if (data_to_free)
                delete[] data_to_free;
}

bool ArchetypeInternal::operator ==(const ArchetypeInternal& other)
{
        return m_Type == other.m_Type;
}

uint32_t
ArchetypeInternal::GetComponentColumn(ComponentId component)
{
        return s_ComponentColumn[GetCAId(component, m_Id)];
}

void
ArchetypeInternal::Reset()
{
        size_t archetype_sizes = s_CreatedArchetypes.size();
        for (unsigned i = 0; i < archetype_sizes; i++) {
                auto& atype = s_CreatedArchetypes[i];
                for (unsigned row = 0; row < atype.m_Entities.size(); row++) {
                        for (auto& c : atype.m_Components) {
                                c.destructor(c[row]);
                        }
                }

                atype.m_Entities.clear();
                s_CreatedArchetypes[i].ChangeSize(5);
        }
}

std::vector<ArchetypeInternal>
ArchetypeInternal::s_CreatedArchetypes = { ArchetypeInternal() };

std::unordered_map<ArchetypeType, Id, __ArchetypeHash>
ArchetypeInternal::s_ArchetypesByType = { { {}, 0 } };

std::unordered_map<uint64_t, uint32_t> ArchetypeInternal::s_ComponentColumn;

uint32_t
Archetype::GetComponentColumn(ComponentId component)
{
        return GetInternal().GetComponentColumn(component);
}

uint32_t
Archetype::AddEntity(EntityId e, World& world)
{
        return GetInternal().AddEntity(e, world, true);
}

uint32_t
Archetype::MoveEntity(EntityId e, Archetype dest, World& world)
{
        ArchetypeInternal& dest_int = dest.GetInternal();
        return GetInternal().MoveEntity(e, dest_int, world);
}

void
Archetype::RemoveEntity(EntityId e, uint32_t row, World& world)
{
        return GetInternal().RemoveEntity(e, row, world, true);
}

ArchetypeInternal&
Archetype::GetInternal()
{
        return ArchetypeInternal::s_CreatedArchetypes[m_Id];
}

EntityId
Archetype::GetEntity(uint32_t index)
{
        return GetInternal().m_Entities[index];
}

uint32_t
Archetype::GetEntitySize()
{
        return GetInternal().m_Entities.size();
}

ComponentColumn&
Archetype::GetComponents(uint32_t component_id)
{
        auto res = ArchetypeInternal::s_ComponentColumn.find(GetCAId(component_id, m_Id));
        if (res == ArchetypeInternal::s_ComponentColumn.end()) {
                std::cerr << "Entity doesnt have this component\n";
        }

        return GetInternal().m_Components[res->second];
}

ComponentColumn&
Archetype::GetComponentsRaw(uint32_t component_index)
{
        return GetInternal().m_Components[component_index];
}

uint32_t
Archetype::GetId()
{
        return m_Id;
}

Archetype
Archetype::GetEdge(ComponentId component, bool is_adding)
{
        return GetInternal().GetEdge(component, is_adding);
}

bool
Archetype::HasComponent(ComponentId component)
{
        return GetInternal().m_Type.contains(component);
}

ComponentColumn&
Archetype::operator [](unsigned component_index)
{
        return GetComponents(component_index);
}

uint32_t
__ArchetypeHash::operator()(const ArchetypeType& type) const
{
        unsigned long key = 5381;

        for (Id cid : type) {
                key ^= cid;
        }

        return key % (0xCAFEBABE / 42069);
}


}  // namespace ECS

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstring>
#include <iostream>

#include "Archetype.h"
#include "World.h"
#include <ostream>

namespace ECS {

using ArchetypeType = std::unordered_set<ComponentId>;

Archetype::Archetype(const std::initializer_list<Id>& type)
        : type(type)
{
}

Archetype::Archetype(const ArchetypeType& type)
        : type(type)
{
}

void Archetype::init(uint32_t id, World& world)
{
        this->id = id;
        this->max_size = 0;

        for (uint32_t i = 0; ComponentId c : type) {
                ComponentData& cdata =
                        world.m_ComponentData[world.m_WorldData[c]
                                                   .component_data_index];

                components.emplace_back(cdata.size, cdata.alignment,
                                        cdata.constructor, cdata.destructor,
                                        cdata.to_s);
                
                world.m_ComponentColumn[world.GetCAId(c, id)] = i;
                i++;
        }
}

ArchetypeId Archetype::get_edge(ComponentId c, World& world, bool is_adding)
{
        if (auto edge = edges.find(c); edge != edges.end()) {
                return edge->second;
        }

        ArchetypeType test = type;
        if (is_adding)
                test.emplace(c);
        else
                test.erase(c);

        ArchetypeId find = world.get_archetype(test);
        edges[c] = find;
        return find;
}

template <class T>
std::ostream& operator <<(std::ostream& str, std::vector<T> vec)
{
        str << "[";
        for (auto& el : vec) {
                str << el << " ";
        }
        str << "]";

        return str;
}
uint32_t Archetype::add_entity(EntityId e, bool init_values)
{
        if (entities.size() >= max_size)
                resize((max_size << 2) + 5);

        uint32_t row = entities.size();
        entities.emplace_back(e);

        if (!init_values)
                return row;

        for (auto& column : components) {
                column.constructor(column[row]);
        }

        return row;
}

uint32_t Archetype::move_entity(EntityId e, Archetype& dest, World& world)
{
        uint32_t dest_row = dest.add_entity(e, false);
        uint32_t from_row = world.m_WorldData[e].archetype_row;
        world.m_WorldData[e].archetype_id = dest.id;
        world.m_WorldData[e].archetype_row = dest_row;

        for (ComponentId c : dest.type) {
                auto& to = dest.components[world.m_ComponentColumn[world.GetCAId(c, dest.id)]];
                auto from_col = world.m_ComponentColumn.find(world.GetCAId(c, id));
                if (from_col == world.m_ComponentColumn.end()) {
                        to.constructor(to[dest_row]);
                } else {
                        std::memcpy(to[dest_row],
                                    components[from_col->second][from_row],
                                    to.element_size);
                }
        }

        remove_entity(e, from_row, world, false);
        return dest_row;
}

void Archetype::remove_entity(EntityId e, uint32_t row, World& world,
                             bool destruct_values)
{
        if (entities.size() == 0)
                return;

        if (destruct_values) {
                for (auto& c : components)
                        c.destructor(c[row]);
        }

        if (entities.size() <= 1) {
                entities.pop_back();
                return;
        }

        uint32_t replaceworldith = entities.size() - 1;
        for (auto& column : components) {
                std::memcpy(column[row], column[replaceworldith],
                            column.element_size);
        }

        world.m_WorldData[entities.back()].archetype_row = row;
        entities.pop_back();

        if (max_size / entities.size() >= 8)
                resize((max_size - 5) >> 2);
}

void Archetype::resize(uint32_t new_size)
{
        uint8_t *data_to_free = max_size == 0 ? nullptr :
                reinterpret_cast<uint8_t*>(components.front().data);

        uint32_t component_sizes = 0;
        for (auto& component : components)
                component_sizes += component.element_size;
        uint8_t* data = new uint8_t[component_sizes * new_size];

        for (uint32_t offset = 0; auto& component : components) {
                uint8_t *new_cdata = &data[offset];
                std::memcpy(new_cdata, component.data,
                            component.element_size * max_size);
                component.data = new_cdata;

                offset += component.element_size * new_size;
        }

        max_size = new_size;

        if (data_to_free)
                delete[] data_to_free;
}

bool Archetype::operator ==(const Archetype& other)
{
        return type == other.type;
}

bool operator ==(std::shared_ptr<Archetype> a, std::shared_ptr<Archetype> b)
{
        return a->type == b->type;
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

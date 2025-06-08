// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "ComponentColumn.h"

namespace ECS {

using Id = uint32_t;
using ComponentId = Id;
using EntityId = Id;
using ArchetypeId = Id;

using ArchetypeType = std::unordered_set<Id>;

struct Archetype {
        uint32_t id;
        uint32_t max_size;
        ArchetypeType type;
        std::vector<ComponentColumn> components;
        std::vector<EntityId> entities;
        std::unordered_map<ComponentId, uint32_t> edges;

        Archetype() : type() {};
        Archetype(const std::initializer_list<Id>& type);
        Archetype(const ArchetypeType& type);
        void init(uint32_t id, class World& world);
        bool operator ==(const Archetype& other);
        ArchetypeId get_edge(ComponentId c, class World& world, bool is_adding);
        uint32_t add_entity(EntityId e, bool init_values = true);
        uint32_t move_entity(EntityId e, Archetype& dest, class World& world);
        void remove_entity(EntityId e, uint32_t row, class World& world,
                           bool destruct_values = true);
        void resize(uint32_t new_size);
        std::string to_s() const {
                std::stringstream s;
                s << "Archetype " << id << " {\n";
                for (auto& ccol : components) {
                        s << "\t" << ccol.to_s(max_size) << "\n";
                }
                s << "}";
                return s.str();
        }
};

bool operator ==(std::shared_ptr<Archetype> a, std::shared_ptr<Archetype> b);

struct __ArchetypeHash {
        uint32_t operator()(const ArchetypeType& type) const;
};

}  // namespace ECS

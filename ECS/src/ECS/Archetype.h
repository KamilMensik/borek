// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "ComponentColumn.h"
#include "Component.h"

namespace ECS {

using Id = uint32_t;
using ComponentId = Id;
using EntityId = Id;

using ArchetypeType = std::unordered_set<Id>;

static constexpr uint64_t
GetCAId(ComponentId c, uint32_t archetype_id) {
        return ((uint64_t)(c) << 32) ^ archetype_id;
}

struct __ArchetypeHash {
        uint32_t operator()(const ArchetypeType& type) const;
};

class ArchetypeInternal {
friend class Archetype;
friend class World;
friend class QueryInternal;
public:
        ArchetypeInternal() = default;
        ArchetypeInternal(const ArchetypeType& type);

        static Id
        Get(const ArchetypeType& type);

        template <typename ... components>
        static Id Get()
        {
                static Id kept_id = UINT32_MAX;
                if (kept_id != UINT32_MAX)
                        return kept_id;

                const ArchetypeType type = { ::ECS::GetId<components>()... };
                kept_id = Get(type);
                return kept_id;
        }

private:
        uint32_t m_Id;
        uint32_t m_MaxSize;
        ArchetypeType m_Type;
        std::vector<ComponentColumn> m_Components;
        std::vector<EntityId> m_Entities;
        std::unordered_map<ComponentId, uint32_t> m_Edges;

        static std::vector<ArchetypeInternal> s_CreatedArchetypes;
        static std::unordered_map<ArchetypeType, Id, __ArchetypeHash> s_ArchetypesByType;
        static std::unordered_map<uint64_t, uint32_t> s_ComponentColumn;

        void
        Init(uint32_t id);

        Id
        GetEdge(ComponentId c, bool is_adding);

        uint32_t
        AddEntity(EntityId e, bool init_values = true);

        uint32_t
        MoveEntity(EntityId e, class ArchetypeInternal& dest);

        void
        RemoveEntity(uint32_t row, bool destruct_values = true);

        void
        ChangeSize(uint32_t new_size);

        static void
        Reset();

        bool
        operator ==(const ArchetypeInternal& other);

        uint32_t
        GetComponentColumn(ComponentId component);
};

class Archetype {
friend class World;
public:
        Archetype() = default;
        Archetype(Id id) : m_Id(id) {}
        
        template <typename ... components>
        static Archetype
        Get()
        {
                static Id kept_id = UINT32_MAX;
                if (kept_id != UINT32_MAX)
                        return Archetype(kept_id);

                kept_id = ArchetypeInternal::Get<components...>();
                return kept_id;
        }

        static Archetype
        Get(const ArchetypeType& type);

        EntityId
        GetEntity(uint32_t index);

        uint32_t
        GetEntitySize();

        uint32_t
        GetComponentColumn(ComponentId component);

        ComponentColumn&
        GetComponents(uint32_t component_id);

        ComponentColumn&
        GetComponentsRaw(uint32_t component_index);

        uint32_t
        GetId();

        Archetype
        GetEdge(ComponentId component, bool is_adding);

        bool
        HasComponent(ComponentId component);

        ComponentColumn& operator [](unsigned component_index);

private:
        Id m_Id;

        uint32_t
        AddEntity(EntityId e);

        uint32_t
        MoveEntity(EntityId e, class Archetype dest);

        void
        RemoveEntity(uint32_t row);

        ArchetypeInternal&
        GetInternal();

        bool operator ==(const ArchetypeInternal& other);
};

}  // namespace ECS

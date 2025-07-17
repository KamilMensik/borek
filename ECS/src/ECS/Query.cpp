// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Query.h"
#include "ECS/Archetype.h"

namespace ECS {

void QueryInternal::TryAdd(const Archetype& archetype)
{
        for (auto& component : m_Type) {
                if (!archetype.type.contains(component))
                        return;
        }

        m_ArchetypeIds.emplace_back(archetype.id);
}

std::vector<QueryInternal>& QueryInternal::GetDefinedQueries()
{
        return s_DefinedQueries;
}

World* QueryInternal::s_World;
std::vector<QueryInternal> QueryInternal::s_DefinedQueries;

}  // namespace ECS

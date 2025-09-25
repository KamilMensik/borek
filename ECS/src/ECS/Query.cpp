// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Query.h"
#include "ECS/Archetype.h"

namespace ECS {

void
QueryInternal::TryAddArchetypeToQueries(Archetype archetype)
{
        for (auto& query : s_DefinedQueries) {
                query.TryAdd(archetype);
        }
}

std::vector<QueryInternal>& QueryInternal::GetDefinedQueries()
{
        return s_DefinedQueries;
}

void QueryInternal::TryAdd(Archetype archetype)
{
        for (auto& component : m_Type) {
                if (!archetype.HasComponent(component))
                        return;
        }

        m_ArchetypeIds.emplace_back(archetype.GetId());
}

std::vector<QueryInternal> QueryInternal::s_DefinedQueries;

}  // namespace ECS

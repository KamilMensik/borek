// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once


#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

#include "ECS/World.h"
#include "ECS/Component.h"

namespace ECS {

using ComponentId = uint32_t;

template <typename...components>
struct View {
        View() = default;

        template<std::size_t index>
        std::tuple_element_t<index, View<components...>> get() &
        {
                return (std::tuple_element_t<index, View<components...>>)items[index];
        }

        template<std::size_t index>
        std::tuple_element_t<index, View<components...>> const get() const&
        {
                return (std::tuple_element_t<index, View<components...>>)items[index];
        }

        template<std::size_t index>
        std::tuple_element_t<index, View<components...>> get() &&
        {
                return std::move((std::tuple_element_t<index, View<components...>>)items[index]);
        }

        template<std::size_t index>
        std::tuple_element_t<index, View<components...>> const get() const&&
        {
                return std::move((std::tuple_element_t<index, View<components...>>)items[index]);
        }

        std::array<void*, sizeof...(components)> items;
};

class QueryInternal {
public:
        template<typename...components>
        static constexpr uint32_t New()
        {
                static uint32_t id = UINT32_MAX;

                if (id == UINT32_MAX) {
                        id = s_DefinedQueries.size();
                        QueryInternal res;
                        res.m_Type = { GetId<components>()... };
                        if (s_World) {
                                for (auto& atype : s_World->m_RegisteredArchetypes) {
                                        res.TryAdd(*atype);
                                }
                        }
                        s_DefinedQueries.emplace_back(res);
                }

                return id;
        }

        template<typename...comps>
        View<comps*...>* Begin()
        {
                m_CurrentArchetypeIndex = UINT32_MAX;
                return Next<comps...>();
        }

        template<typename...comps>
        View<comps*...>* Next()
        {
                if (m_ArchetypeIds.size() == 0)
                        return nullptr;

                static std::array<uint32_t, sizeof...(comps)> cid_in_arch;
                static std::shared_ptr<Archetype> curr;
                static View<comps*...> res;

                if (m_CurrentArchetypeIndex == UINT32_MAX) {
                        m_CurrentArchetypeIndex = 0;
                        m_CurrentIndexInArchetype = 0;
                        curr = s_World->m_RegisteredArchetypes[m_ArchetypeIds[0]];
                        cid_in_arch = { s_World->GetCColumn(ECS::GetId<comps>(), m_ArchetypeIds[m_CurrentArchetypeIndex])... };
                }

                if (m_CurrentIndexInArchetype >= curr->entities.size()) {
                        if (++m_CurrentArchetypeIndex >= m_ArchetypeIds.size()) {
                                return nullptr;
                        }

                        m_CurrentIndexInArchetype = 0;
                        cid_in_arch = { s_World->GetCColumn(ECS::GetId<comps>(), m_ArchetypeIds[m_CurrentArchetypeIndex])... };
                        curr = s_World->m_RegisteredArchetypes[m_ArchetypeIds[0]];
                }
                
                for (size_t i = 0; i < cid_in_arch.size(); i++) {
                        res.items[i] = curr->components[cid_in_arch[i]][m_CurrentIndexInArchetype];
                }

                m_CurrentIndexInArchetype++;

                return &res;
        }

        void TryAdd(const Archetype& archetype);
        static void SetWorld(World* world) { s_World = world; }

        static std::vector<QueryInternal>& GetDefinedQueries();

private:
        uint32_t m_CurrentArchetypeIndex = UINT32_MAX;
        uint32_t m_CurrentIndexInArchetype = 0;

        std::vector<ComponentId> m_Type;
        std::vector<uint32_t> m_ArchetypeIds;
        
        static World* s_World;
        static std::vector<QueryInternal> s_DefinedQueries;
};

template<typename...components>
class Query {
public:
        struct Iterator {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = View<components*...>;
                using pointer = value_type*;
                using reference = value_type&;

                Iterator(pointer p, uint32_t query_index) : m_QueryIndex(query_index), m_Ptr(p) {}


                reference operator*() const { return *m_Ptr; }
                pointer operator->() { return m_Ptr; }

                Iterator& operator++() {
                        m_Ptr = QueryInternal::GetDefinedQueries()[m_QueryIndex].Next<components...>();
                        return *this;
                }

                friend bool operator== (const Iterator& a, const Iterator& b)
                {
                        return a.m_Ptr == nullptr && b.m_Ptr == nullptr;
                }

                friend bool operator!= (const Iterator& a, const Iterator& b)
                {
                        return a.m_Ptr != nullptr || b.m_Ptr != nullptr;
                }
                
                uint32_t m_QueryIndex;
                pointer m_Ptr;
        };

        constexpr Query()
        {
                m_QueryInternalId = QueryInternal::New<components...>();
        }

        Iterator begin() {
                return Iterator(QueryInternal::GetDefinedQueries()[m_QueryInternalId].Begin<components...>(), m_QueryInternalId);
        }

        Iterator end()
        {
                return Iterator(nullptr, m_QueryInternalId);
        }

private:
        uint32_t m_QueryInternalId;
};

}  // namespace ECS

namespace std {
        template<typename...components>
        struct tuple_size<::ECS::View<components*...>>
        {
                static constexpr size_t value = sizeof...(components);
        };

        template<size_t n, typename...components>
        struct tuple_element<n, ::ECS::View<components*...>>
        {
                using type = std::tuple_element_t<n, std::tuple<components*...>>;
        };
}

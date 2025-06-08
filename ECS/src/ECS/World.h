#pragma once

/*
 *      ECS system by Kamil Mensik (kamilekmensik@gmail.com)
 *      Made for Borek game engine. (https://github.com/KamilMensik/borek)
 *      Copyright 2024-2025 <kamilekmensik@gmail.com>
 *
 *      ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 *      ;;;;                            Usage:                             ;;;;
 *      ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 *      
 *      
 *      ECS::World
 *      =======================================================================
 *      # World is storing all ECS data.
 *      # Cleans itself upon deletion.
 *
 *      -----------------------------------------------------------------------
 *      ==> Creating a world:
 *
 *      # Creates a new ECS world
 *      ECS::World world;
 *      =======================================================================
 *
 *      
 *      ECS::Entity
 *      =======================================================================
 *      # Entities are composed from components.
 *      # They itself do not store any data. (Entity is just an uint32_t)
 *
 *      -----------------------------------------------------------------------
 *      ==> Creating entities
 *
 *      # Creating an empty entity
 *      ECS::Entity entity = world.entity();
 *
 *      # Creating entity with components already registered
 *      # Passing raw component list is supported, but not recommended.
 *      # This is exponentially slower than using the second variant.
 *      ECS::Entity entity = world.entity({ Sprite, Transform }); 
 *
 *      # Creating an entity using a predetermined archetype.
 *      ECS::Entity entity = world.entity(PlayerArchetype);
 *
 *      -----------------------------------------------------------------------
 *      ==> Removing an entity:
 *
 *      # Removes the entity and its components.
 *      world.remove(entity);
 *      -----------------------------------------------------------------------
 *      ==> Manipulating the components of entity
 *
 *      # Setting components: (overriding them)
 *      # There are two ways to do it:
 *      # The slow variant. That should not be used.
 *      world.set_components(entity, { componentA, componentB});
 *
 *      # The exponentially faster variant, that is much preffered.
 *      world.set_components(entity, PlayerArchetype);
 *
 *      # Adding a single component:
 *      # This is O(1), Slower if world needs to generate new Archetype.
 *      world.add_component(entity, componentA);
 *      -----------------------------------------------------------------------
 *      ==> Removing components from entity
 *
 *      # Removes a single component.
 *      # This is O(1), Slower if world needs to generate new Archetype.
 *      world.remove_component(entity, componentA);
 *      -----------------------------------------------------------------------
 *      ==> Working with components
 *
 *      # Checking whether entity has a component
 *      world.has_component(entity, transform);
 *
 *      # Getting a component
 *      world.get_component(entity, transform) => returns void* to component
 *
 *      # Automatically casting component to specific structure
 *      world.get_component<Transform>(entity, transform) => returns Transform&
 *
 *      # This way you can manipulate components without explicit set methods
 *      =======================================================================
 *
 *
 *      ECS::Component
 *      =======================================================================
 *      # Used to store data of entity.
 *      # Creating a component doesnt automatically use memory.
 *      # Memory is used only when it is a part of Archetype (Entity)
 *
 *      -----------------------------------------------------------------------
 *      ==> Creating a component
 *
 *      # Defining a struct for the component layout
 *      struct Position {
 *              int x, int y;
 *      };
 *
 *      # Returns a new component that represents the Position struct;
 *      ECS::Component component = world.component<Position>();
 *
 *      # Create components with implicitly specified information
 *      # about the data that is being stored inside the component.
 *      world.component(data_size, data_alignment);
 *
 *      ******************************WARNING**********************************
 *      # p1 and p2 are completely different components.
 *      # They just have the same memory layout.
 *      # If you wish to reuse components, you need to store them.
 *
 *      ECS::Component p1 = world.component<Position>;
 *      ECS::Component p2 = world.component<Position>;
 *      (p1 == p2) => returns false
 *      ***********************************************************************
 *
 *      -----------------------------------------------------------------------
 *      ==> Removing components
 *
 *      # You can remove components from entities.
 *      # But removing components completely is not supported.
 *      =======================================================================
 *
 *
 *      ECS::Archetype
 *      =======================================================================
 *      # Archetype is a group of entities with the same component layout.
 *      # Components are position independent
 *      ({ Transform, HP} == { HP, Transform}) => true
 *
 *      -----------------------------------------------------------------------
 *      ==> Creating a new archetype
 *
 *      # Creates a new / returns an existing archetype for given components
 *      # Really slow operation. If you want to reuse it. Save it somewhere.
 *      ECS::Archetype enemy = world.get_archetype({ HP, Transform, Sprite });
 *
 */

#include <concepts>
#include <cstdint>
#include <vector>
#include <memory>
#include <iostream>
#include <typeinfo>

#include "Archetype.h"
#include "Component.h"
#include "Query.h"

namespace ECS {


using Id = uint32_t;
using EntityId = Id;
using ComponentId = Id;
using ArchetypeId = Id;

using __AType_IList = std::initializer_list<Id>;

struct WData {
        union {
                struct { uint32_t archetype_id; uint32_t archetype_row; };
                uint32_t component_data_index;
        };
};

class World {
friend Archetype;
friend QueryL;
friend Query;
public:
        World();

        /*
         *      Entities
         */
        
        // Entity creation
        EntityId entity();
        EntityId entity(const __AType_IList& components);
        EntityId entity(ArchetypeId archetype);
        void remove(EntityId);

        // Setting components
        void set_components(EntityId entity, const __AType_IList& components);
        void set_components(EntityId entity, ArchetypeId archetype);
        template <class T> requires std::derived_from<T, Component<T>>
        inline void add_component(EntityId entity)
        {
                add_component(entity, T::component_id);
        }
        void add_component(EntityId entity, ComponentId component);
        template <class T> requires std::derived_from<T, Component<T>>
        inline void remove_component(EntityId entity)
        {
                remove_component(entity, T::component_id);
        }
        void remove_component(EntityId entity, ComponentId component);

        // Getting components
        bool has_component(EntityId, ComponentId);
        void* get_component(EntityId, ComponentId);
        template <class T>
        T& get_component(EntityId e, ComponentId c) {
                return *reinterpret_cast<T*>(get_component(e, c));
        }
        template <class T> requires std::derived_from<T, Component<T>>
        T& get_component(EntityId e) {
                return *reinterpret_cast<T*>(get_component(e, T::component_id));
        }

        /*
         *      Components
         */

        // Create Components
        template <class T>
        requires std::derived_from<T, Component<T>>
        ComponentId component()
        {
                if (T::component_id != UINT32_MAX)
                        return T::component_id;

                void (*constructor)(void*) = [](void* address){
                        new(address) T();
                };
                void (*destructor)(void*) = [](void* address){
                        reinterpret_cast<T*>(address)->~T();
                };
                T::component_id = component(sizeof(T), alignof(T),
                                            constructor, destructor);
                return T::component_id;
        }
        ComponentId component(uint32_t size, uint32_t alignment,
                              void(*constructor)(void*),
                              void(*destructor)(void*));

        // Archetypes
        // Extremely slow. Try to save result somewhere.
        ArchetypeId get_archetype(const __AType_IList& type);
        Query new_query(const std::initializer_list<ComponentId>& components);

public:
        uint32_t m_CurrentId;
        std::vector<WData> m_WorldData;
        std::vector<Id> m_Reusable;
        std::unordered_map<uint64_t, uint32_t> m_ComponentColumn;
        std::vector<std::shared_ptr<Archetype>> m_RegisteredArchetypes;
        std::unordered_map<ArchetypeType, uint32_t, __ArchetypeHash> m_Archetypes;
        std::vector<ComponentData> m_ComponentData;
        std::vector<QueryL> m_LQueries;

        Id GetId();
        constexpr uint64_t GetCAId(ComponentId c, uint32_t archetype_id) {
                return ((uint64_t)(c) << 32) ^ archetype_id;
        }
        uint32_t GetCColumn(uint32_t component_id, uint32_t archetype_id);

        // @param [Archetype&] must be top of m_RegisteredArchetypes
        // @returns [Archetype&] either supplied archetype, or already existing one with same type.
        ArchetypeId get_archetype(ArchetypeType& type);
};

}  // namespace ECS

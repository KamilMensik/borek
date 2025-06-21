// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "box2d/id.h"
#include <concepts>
#include <utility>

#include <ECS/World.h>
#include <ECS/Archetype.h>

#include "Include/Base/Entity.h"
#include "Include/Base/Component.h"
#include "Include/Base/Query.h"

#pragma once

namespace Borek {

class Scene {
friend Entity;
friend class SceneSerializer;
friend class Application;
public:
        struct BaseQueries {
                Borek::Query entity_query;
                Borek::Query draw_query;
                Borek::Query camera_query;
                Borek::Query scriptable_object_query;
                Borek::Query rigidbody_query;
        };

        Scene();

        Entity NewEntity(const std::string& tag = "");
        void DeleteEntity(Entity e);

        inline const BaseQueries& GetBaseQueries() const { return m_Queries; }

        // Call only once on each component, or previous iteration will be
        // corrupted.
        template <class T> requires std::derived_from<T, Component<T>>
        void RegisterComponent() {
                T::component_id = UINT32_MAX;
                m_World.component<T>();
        }

        template <typename... Args>
        Query Query(Args... args)
        {
                return m_World.new_query({std::forward<Args>(args)...});
        }

private:
        ECS::World m_World;
        BaseQueries m_Queries;
        b2WorldId m_PhysicsWorld;

        void RegisterBaseQueries();
        void RegisterBaseComponents();
};

}  // namespace Borek

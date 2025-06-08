// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ECS/Archetype.h"
#include "Include/Base/Component.h"
#include "Include/Base/Query.h"
#include <concepts>

#include <ECS/World.h>

#include "Entity.h"
#include <utility>

#pragma once

namespace Borek {

class Scene {
friend Entity;
friend class SceneSerializer;
public:
        Scene();

        Entity NewEntity(const std::string& tag = "");
        void DeleteEntity(Entity e);

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
};

}  // namespace Borek

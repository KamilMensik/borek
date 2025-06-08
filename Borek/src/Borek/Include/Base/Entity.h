// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Component.h"
#include "ECS/Archetype.h"
#include "Include/Core.h"
#include <concepts>
#include <cstdint>

#pragma once

namespace Borek {

class Entity {
friend class Scene;
public:
        Entity() : m_Id(UINT32_MAX) {}
        Entity(ECS::EntityId id, class Scene* scene);

        bool operator ==(const Entity& other)
        {
                return m_Id == other.m_Id && m_Scene == other.m_Scene;
        }

        bool operator !=(const Entity& other)
        {
                return m_Id != other.m_Id || m_Scene != other.m_Scene;
        }

        template <class T> requires std::derived_from<T, Component<T>>
        T& GetComponent() { return *RCAST<T*>(GetComponent(T::component_id)); }
        void* GetComponent(ECS::ComponentId cid);

        template <class T> requires std::derived_from<T, Component<T>>
        Entity& AddComponent() { return AddComponent(T::component_id); }
        template <class T, typename ...CArgs>
        requires std::derived_from<T, Component<T>>
        Entity& AddComponent(CArgs ...args)
        {
                AddComponent(T::component_id);
                GetComponent<T>() = T(std::forward<CArgs>(args)...);
                return *this;
        }
        Entity& AddComponent(ECS::ComponentId cid);

        template <class T> requires std::derived_from<T, Component<T>>
        Entity& RemoveComponent() { return RemoveComponent(T::component_id); }
        Entity& RemoveComponent(ECS::ComponentId cid);
        void Delete();

        template <class T> requires std::derived_from<T, Component<T>>
        bool HasComponent() { return HasComponent(T::component_id); }
        bool HasComponent(ECS::ComponentId cid);
        bool IsNil() { return m_Id == UINT32_MAX; }
        inline uint32_t GetId() const { return m_Id; }

private:
        class Scene* m_Scene;
        ECS::EntityId m_Id;
};

}  // namespace Borek

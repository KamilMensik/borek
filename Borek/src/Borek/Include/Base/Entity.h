// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <ECS/Archetype.h>
#include <ECS/World.h>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Base/UUID.h"

#pragma once

namespace Borek {

struct TransformComponent;

class Entity {
friend class Scene;
public:
        enum class Type : uint32_t {
                Node,
        };

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

        template <class T>
        T& GetComponent() { return *RCAST<T*>(GetComponent(ECS::GetId<T>())); }
        void* GetComponent(ECS::ComponentId cid);

        template <class T>
        Entity& AddComponent() { return AddComponent(ECS::GetId<T>()); }
        template <class T, typename ...CArgs>
        Entity& AddComponent(CArgs ...args)
        {
                AddComponent(ECS::GetId<T>());
                GetComponent<T>() = T(std::forward<CArgs>(args)...);
                return *this;
        }
        Entity& AddComponent(ECS::ComponentId cid);

        template <class T>
        Entity& RemoveComponent() { return RemoveComponent(ECS::GetId<T>()); }
        Entity& RemoveComponent(ECS::ComponentId cid);
        void Delete();

        template <class T>
        bool HasComponent() { return HasComponent(ECS::GetId<T>()); }
        bool HasComponent(ECS::ComponentId cid);
        bool IsNil() { return m_Id == UINT32_MAX; }
        inline uint32_t GetId() const { return m_Id; }
        operator uint32_t() const { return GetId(); }

        TransformComponent& Transform();
        const char* GetName();
        UUID GetUUID();

public:
        class Scene* m_Scene;
        ECS::EntityId m_Id;
        Type m_Type;
};

}  // namespace Borek

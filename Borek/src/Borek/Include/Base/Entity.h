// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Node.h"
#include "glm/ext/vector_float2.hpp"
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
        Entity() : m_Id(UINT32_MAX) {}
        Entity(ECS::EntityId id);

        bool operator ==(const Entity& other)
        {
                return m_Id == other.m_Id;
        }

        bool operator !=(const Entity& other)
        {
                return m_Id != other.m_Id;
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
        const TransformComponent GlobalTransform();
        const char* GetName();
        UUID GetUUID();

        bool
        HasParent();

        Entity
        GetParent();

        bool
        HasChildren();

        NodeType
        GetNodeType();

        // @return [bool] - Returns true if node type changed.
        bool
        InitializeNode(NodeType type);

        void
        DeinitializeNode();

        /*
         * Physics engine API
         */

        //
        // ENTITY NEEDS TO BE CHARACTER BODY!
        //
        glm::vec2
        MoveAndCollide(float x, float y);

public:
        ECS::EntityId m_Id;
};

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <glm/ext/vector_float2.hpp>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Base/Node.h"
#include "Include/Base/UUID.h"
#include "Include/Components/TransformComponent.h"

#pragma once

namespace Borek {

class Entity {
friend class Scene;
public:
        Entity() : m_Id(UINT32_MAX) {}
        Entity(uint32_t id);

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
        void* GetComponent(uint32_t cid);

        template <class T>
        Entity& AddComponent() { return AddComponent(ECS::GetId<T>()); }
        template <class T, typename ...CArgs>
        Entity& AddComponent(CArgs ...args)
        {
                AddComponent(ECS::GetId<T>());
                GetComponent<T>() = T(std::forward<CArgs>(args)...);
                return *this;
        }
        Entity& AddComponent(uint32_t cid);

        template <class T>
        Entity& RemoveComponent() { return RemoveComponent(ECS::GetId<T>()); }
        Entity& RemoveComponent(uint32_t cid);
        void Delete();

        template <class T>
        bool HasComponent() { return HasComponent(ECS::GetId<T>()); }
        bool HasComponent(uint32_t cid);
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
        IsParentOf(Entity e);

        bool
        HasChildren();

        std::vector<uint32_t>*
        GetChildren();

        void
        DeleteChildren();

        Entity
        FindChild(const std::string& name);

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
        uint32_t m_Id;
};

}  // namespace Borek

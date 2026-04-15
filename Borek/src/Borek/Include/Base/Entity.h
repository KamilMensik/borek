// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Symbol.h"
#include "Include/Engine/FZX/Collision.h"
#include <cstdint>

#include <glm/ext/vector_float2.hpp>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Base/Node.h"
#include "Include/Base/UUID.h"
#include "Include/Components/TransformComponent.h"
#include <string_view>

#pragma once

namespace Borek {

class Entity {
friend class Scene;
public:
        Entity() : m_Id(UINT32_MAX) {}
        Entity(uint32_t id);

        bool operator ==(const Entity& other) const
        {
                return m_Id == other.m_Id;
        }

        bool operator !=(const Entity& other) const
        {
                return m_Id != other.m_Id;
        }

        template <class T>
        T& GetComponent() { return *RCAST<T*>(GetComponent(ECS::GetId<T>())); }
        void* GetComponent(uint32_t cid);

        template <class T>
        const T& GetComponentC() const { return *RCAST<const T*>(GetComponentC(ECS::GetId<T>())); }
        const void* GetComponentC(uint32_t cid) const;

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
        bool HasComponent() const { return HasComponent(ECS::GetId<T>()); }
        bool HasComponent(uint32_t cid) const;
        bool IsNil() const { return m_Id == UINT32_MAX; }
        inline uint32_t GetId() const { return m_Id; }
        operator uint32_t() const { return GetId(); }

        TransformComponent& Transform();
        const TransformComponent GlobalTransform() const;
        const char* GetName() const;
        Symbol GetNameSym() const;
        void SetNameSym(Symbol& name);
        UUID GetUUID() const;

        bool
        HasParent() const;

        Entity
        GetParent() const;

        Entity
        GetPrevSibling() const;

        Entity
        GetNextSibling() const;

        bool
        IsParentOf(Entity e) const;

        bool
        HasChildren() const;

        const std::vector<Entity>*
        GetChildren() const;

        void
        DeleteChildren();

        Entity
        FindChild(Symbol name) const;

        NodeType
        GetNodeType() const;

        uint32_t
        ParentCount() const;

        uint64_t
        GetRubyNode() const;

        // @return [bool] - Returns true if node type changed.
        bool
        InitializeNode(NodeType type);

        void
        DeinitializeNode();

        std::string
        PathTo(Entity other) const;

        std::string
        GetAbsolutePath() const;

        static Entity
        FindFromAbsolutePath(std::string_view path);

        Entity
        FindEntityByPath(std::string_view path) const;

        /*
         * Physics engine API
         */

        //
        // ENTITY NEEDS TO BE CHARACTER BODY!
        //
        std::pair<FZX::Collision, glm::vec2>
        MoveAndCollide(float x, float y);


public:
        uint32_t m_Id;
};

}  // namespace Borek

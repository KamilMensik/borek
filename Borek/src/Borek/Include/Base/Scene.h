// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Node.h"
#include "Include/Engine/FZX/FACD.h"
#include "Include/Engine/Utils/PairHasher.h"
#include <unordered_map>
#include <functional>

#include <ECS/World.h>
#include <ECS/Component.h>

#include "Include/Base/Entity.h"

#pragma once

namespace Borek {

class Scene {
friend Entity;
friend class SceneSerializer;
friend class Application;
friend class EntityInitializer;
using iteration_func = std::function<void(Entity)>;
public:
        Scene();
        ~Scene();

        void
        Init();

        Entity
        NewEntity(const std::string& tag = "", NodeType type = NodeType::Node);

        void
        ChangeEntityNodeType(Entity e, NodeType type);

        void
        DeleteEntity(Entity e);

        Entity&
        RootEntity();

        // Scene graph API
        void
        TraverseScene(iteration_func iter_func);

        void
        TraverseSceneReverse(iteration_func iter_func);

        void
        TraverseScene(iteration_func before, iteration_func after);

        void
        TraverseSceneReverse(iteration_func before, iteration_func after);

        Entity
        EntityFindFirstChild(const std::string& name, Entity parent = Entity());

        void
        EntityAppendChild(Entity entity, Entity dest);

        void
        EntityPrependChild(Entity entity, Entity dest);

        void
        EntityAppend(Entity entity, Entity dest);

        void
        EntityPrepend(Entity entity, Entity dest);

        Entity
        GetEntityParent(Entity entity);

        bool
        HasEntityChildren(Entity entity);

        const std::vector<uint32_t>*
        GetEntityChildren(Entity entity);

        NodeType
        GetEntityNodeType(Entity e);

        FZX::FACD&
        GetPhysicsWorld();

public:
        Uniq<ECS::World> m_World;
        Entity m_RootEntity;
        FZX::FACD m_PhysicsWorld;

        std::unordered_map<uint32_t, std::vector<uint32_t>>
        m_SceneGraph;
        std::unordered_map<uint32_t, uint32_t>
        m_EntityParents;
        std::unordered_map<std::pair<std::string, uint32_t>, uint32_t, PairHash>
        m_SceneGraphByName;
        std::unordered_map<uint32_t, NodeType>
        m_EntityNodeTypes;

        void
        TraverseSceneHelper(Entity e, std::function<void(Entity e)> iter_func);

        void
        TraverseSceneReverseHelper(Entity e, std::function<void(Entity e)> iter_func);

        void
        TraverseSceneHelper(Entity e, iteration_func before,
                            iteration_func after);

        void
        TraverseSceneReverseHelper(Entity e, iteration_func before,
                                   iteration_func after);

        void
        RemoveEntityFromGraph(Entity e);

        void
        RemoveEntityFromSceneGraph(Entity e);

        uint32_t
        GetEntityParentId(Entity e);

        template <class T> T&
        GetComponent(Entity e)
        {
                return *RCAST<T*>(m_World->get_component(e, ECS::GetId<T>()));
        }
};

}  // namespace Borek

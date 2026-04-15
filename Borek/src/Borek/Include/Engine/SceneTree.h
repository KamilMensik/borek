// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string_view>

#include "Include/Core.h"
#include "Include/Base/Symbol.h"
#include "Include/Engine/Utils/PairHasher.h"
#include "Include/Base/Node.h"
#include "Include/Base/Entity.h"

namespace Borek {

class SceneTree {
private:
        using iteration_func = std::function<void(Entity)>;

public:
        void
        AddEntity(Entity e, const Symbol& name, NodeType type,
                  Entity parent = Entity());

        void
        RemoveEntity(Entity e);

        void
        TraverseScene(iteration_func iter_func) const;

        void
        TraverseSceneReverse(iteration_func iter_func) const;

        void
        TraverseScene(iteration_func before, iteration_func after) const;

        void
        TraverseSceneReverse(iteration_func before, iteration_func after) const;

        void
        TraverseScene(Entity e, iteration_func iter_func) const;

        void
        TraverseSceneReverse(Entity e, iteration_func iter_func) const;

        void
        TraverseScene(Entity e, iteration_func before,
                      iteration_func after) const;

        void
        TraverseSceneReverse(Entity e, iteration_func before,
                             iteration_func after) const;


        Entity
        EntityFindChild(Symbol name, Entity parent = Entity()) const;

        void
        EntityAppendChild(Entity entity, Entity dest);

        void
        EntityPrependChild(Entity entity, Entity dest);

        void
        EntityAppend(Entity entity, Entity dest);

        void
        EntityPrepend(Entity entity, Entity dest);

        Entity
        GetRootEntity() const;

        void
        SetRootEntity(Entity root);

        Entity
        GetEntityParent(Entity e) const;

        uint32_t
        GetEntityParentCount(Entity e) const;

        Entity
        GetEntityPrevSibling(Entity e) const;

        Entity
        GetEntityNextSibling(Entity e) const;

        const Symbol&
        GetEntityName(Entity e) const;

        void
        SetEntityName(Entity e, const Symbol& sym, bool force = false);

        NodeType
        GetEntityNodeType(Entity e) const;

        void
        SetEntityNodeType(Entity e, NodeType type);

        uint64_t
        GetEntityRubyNode(Entity e) const;

        void
        SetEntityRubyNode(Entity e, uint64_t ruby_node);

        uint64_t
        GetEntityCount() const;

        const std::vector<Entity>*
        GetEntityChildren(Entity e) const;

        bool
        HasEntityChildren(Entity e) const;

        bool
        IsEntityParentOf(Entity a, Entity b) const;

        std::string
        PathTo(Entity a, Entity b) const;

        Entity
        FindEntityByPath(Entity from, std::string_view path) const;

        std::string
        ToS() const;

        // Deletes whole graph + also deletes all entities
        void
        Destroy(const std::function<void(Entity)>& fn) &&;

        // Removes entity and its children from the graph
        void
        DestroyEntity(Entity e, Entity parent = Entity());

private:
        struct EntityData {
                uint64_t ruby_node;
                Entity parent = Entity();
                NodeType node_type;
                Symbol name;
                uint32_t parent_count;
        };

        Map<uint32_t, std::vector<Entity>> m_SceneGraph;
        Map<uint32_t, EntityData> m_EntityData;
        Map<std::pair<Symbol, uint32_t>, Entity, PairHash> m_SceneGraphByName;
        Entity m_RootEntity;

        void
        ReparentEntity(Entity e, Entity parent, EntityData& ed);

        void
        Destroy(Entity e, const std::function<void(Entity)>& fn);
};

}  // namespace Borek

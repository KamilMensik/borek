// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <format>
#include <ranges>

#include <ECS/Archetype.h>
#include <ECS/World.h>
#include <ECS/Component.h>

#include "Include/Base/Scene.h"
#include "Include/Base/Components.h"
#include "Include/Debug/Log.h"

namespace Borek {

static ECS::Archetype base_entity;

Scene::Scene()
{
        base_entity = ECS::Archetype::Get<
                IDComponent, TransformComponent, TagComponent
        >();
}

Scene::~Scene()
{
}

void
Scene::Init()
{
        m_World = NewUniq<ECS::World>();
        m_RootEntity = NewEntity("Root");
}

Entity
Scene::NewEntity(const std::string& value, NodeType type)
{
        Entity e(m_World->entity(base_entity));
        m_EntityNodeTypes[e] = NodeType::Node;

        TagComponent& tc = GetComponent<TagComponent>(e);
        GetComponent<IDComponent>(e).ecs_id = e.GetId();

        tc.value = std::string(value.empty() ? std::format("Entity {}", e.GetId()) : value);

        e.InitializeNode(type);

        return e;
}

void
Scene::ChangeEntityNodeType(Entity e, NodeType type)
{
        e.InitializeNode(type);
}


void
Scene::DeleteEntity(Entity e)
{
        RemoveEntityFromGraph(e);
        m_EntityNodeTypes.erase(e);
        m_World->remove(e.m_Id);
}

Entity&
Scene::RootEntity()
{
        return m_RootEntity;
}

void
Scene::TraverseScene(std::function<void(Entity e)> iter_func)
{
        TraverseSceneHelper(m_RootEntity, iter_func);
}

void
Scene::TraverseSceneReverse(std::function<void(Entity e)> iter_func)
{
        TraverseSceneReverseHelper(m_RootEntity, iter_func);
}

void
Scene::TraverseScene(iteration_func before, iteration_func after)
{
        TraverseSceneHelper(m_RootEntity, before, after);
}

void
Scene::TraverseSceneReverse(iteration_func before, iteration_func after)
{
        TraverseSceneReverseHelper(m_RootEntity, before, after);
}

Entity
Scene::EntityFindFirstChild(const std::string& name, Entity parent)
{
        if (parent.IsNil())
                parent = m_RootEntity;

        auto children = m_SceneGraphByName.find({ name, parent });
        if (children == m_SceneGraphByName.end())
                return Entity();

        return Entity(children->second);
}

void
Scene::EntityAppendChild(Entity entity, Entity dest)
{
        if (GetEntityParentId(entity) != UINT32_MAX) {
                if (GetEntityParentId(entity) != dest) {
                        RemoveEntityFromGraph(entity);
                } else {
                        RemoveEntityFromSceneGraph(entity);
                }
        }

        m_EntityParents[entity] = dest;
        m_SceneGraph[dest].emplace_back(entity);
        m_SceneGraphByName[{entity.GetName(), dest}] = entity;
}

void
Scene::EntityPrependChild(Entity entity, Entity dest)
{
        uint32_t dest_parent_id = dest;

        if (GetEntityParentId(entity) != UINT32_MAX) {
                if (GetEntityParentId(entity) != dest_parent_id) {
                        RemoveEntityFromGraph(entity);
                } else {
                        RemoveEntityFromSceneGraph(entity);
                }
        }

        auto it = m_SceneGraph[dest_parent_id].begin();
        m_EntityParents[entity] = dest_parent_id;
        m_SceneGraph[dest_parent_id].insert(it, entity);
        m_SceneGraphByName[{entity.GetName(), dest_parent_id}] = entity;
}

void
Scene::EntityAppend(Entity entity, Entity dest)
{
        uint32_t dest_parent_id = GetEntityParentId(entity);

        if (GetEntityParentId(entity) != UINT32_MAX) {
                if (GetEntityParentId(entity) != dest_parent_id) {
                        RemoveEntityFromGraph(entity);
                } else {
                        RemoveEntityFromSceneGraph(entity);
                }
        }

        auto it = std::find(m_SceneGraph[dest_parent_id].begin(),
                            m_SceneGraph[dest_parent_id].end(),
                            dest);

        m_EntityParents[entity] = dest_parent_id;
        m_SceneGraph[dest_parent_id].insert(it + 1, entity);
        m_SceneGraphByName[{entity.GetName(), dest_parent_id}] = entity;
}

void
Scene::EntityPrepend(Entity entity, Entity dest)
{
        uint32_t dest_parent_id = GetEntityParentId(entity);

        if (GetEntityParentId(entity) != UINT32_MAX) {
                if (GetEntityParentId(entity) != dest_parent_id) {
                        RemoveEntityFromGraph(entity);
                } else {
                        RemoveEntityFromSceneGraph(entity);
                }
        }

        auto it = std::find(m_SceneGraph[dest_parent_id].begin(),
                            m_SceneGraph[dest_parent_id].end(),
                            dest);

        m_EntityParents[entity] = dest_parent_id;
        m_SceneGraph[dest_parent_id].insert(it, entity);
        m_SceneGraphByName[{entity.GetName(), dest_parent_id}] = entity;
}

Entity
Scene::GetEntityParent(Entity entity)
{
        return Entity(GetEntityParentId(entity));
}

bool
Scene::HasEntityChildren(Entity entity)
{
        auto children = m_SceneGraph.find(entity);

        return children != m_SceneGraph.end() && children->second.size() > 0;
}

const std::vector<uint32_t>*
Scene::GetEntityChildren(Entity entity)
{
        auto children = m_SceneGraph.find(entity);
        if (children == m_SceneGraph.end())
                return nullptr;

        return &children->second;

}

NodeType
Scene::GetEntityNodeType(Entity e)
{
        return m_EntityNodeTypes[e];
}

void
Scene::TraverseSceneHelper(Entity e, std::function<void(Entity e)> iter_func)
{
        iter_func(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : children->second) {
                        Entity e(entity_id);
                        TraverseSceneHelper(e, iter_func);
                }
        }
}

void
Scene::TraverseSceneReverseHelper(Entity e, std::function<void(Entity e)> iter_func)
{
        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        Entity e(entity_id);
                        TraverseSceneHelper(e, iter_func);
                }
        }

        iter_func(e);
}

void
Scene::TraverseSceneHelper(Entity e, iteration_func before,
                           iteration_func after)
{
        before(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        Entity e(entity_id);
                        TraverseSceneHelper(e, before, after);
                }
        }

        after(e);
}

void
Scene::TraverseSceneReverseHelper(Entity e, iteration_func before,
                                  iteration_func after)
{
        before(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        Entity e(entity_id);
                        TraverseSceneHelper(e, before, after);
                }
        }

        after(e);
}

void
Scene::RemoveEntityFromSceneGraph(Entity e)
{
        uint32_t parent_id = GetEntityParentId(e);
        auto it = std::find(m_SceneGraph[parent_id].begin(),
                            m_SceneGraph[parent_id].end(), e);

        m_SceneGraph[parent_id].erase(it);
}

void
Scene::RemoveEntityFromGraph(Entity e)
{
        RemoveEntityFromSceneGraph(e);
        uint32_t parent_id = GetEntityParentId(e);
        m_EntityParents.erase(e);
        m_SceneGraphByName.erase({e.GetName(), parent_id});
}

uint32_t
Scene::GetEntityParentId(Entity e)
{
        if (auto p = m_EntityParents.find(e); p != m_EntityParents.end())
                return p->second;

        return UINT32_MAX;
}

FZX::FACD&
Scene::GetPhysicsWorld()
{
        return m_PhysicsWorld;
}

}  // namespace Borek

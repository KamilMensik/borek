// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <ECS/Archetype.h>

#include "Include/Base/Scene.h"
#include "Include/Base/Components.h"
#include <format>

namespace Borek {

static ECS::ArchetypeId base_entity;

Entity Scene::NewEntity(const std::string& value)
{
        Entity e(m_World.entity(base_entity), this);

        TagComponent& tc = e.GetComponent<TagComponent>();

        tc.value = std::string(value.empty() ? std::format("Entity {}", e.GetId()) : value);

        return e;
}

void Scene::DeleteEntity(Entity e)
{
        m_World.remove(e.m_Id);
}

Scene::Scene()
{
        RegisterBaseComponents();
        RegisterBaseQueries();

        base_entity = m_World.get_archetype({ TransformComponent::Id(),
                                              TagComponent::Id() });
}

void Scene::RegisterBaseComponents()
{
        RegisterComponent<TransformComponent>();
        RegisterComponent<TagComponent>();
        RegisterComponent<SpriteComponent>();
        RegisterComponent<CameraComponent>();
        RegisterComponent<ScriptComponent>();
        RegisterComponent<RigidBody2DComponent>();
        RegisterComponent<BoxCollider2DComponent>();
}

void Scene::RegisterBaseQueries()
{
        m_Queries.entity_query = Query(TagComponent::Id());
        m_Queries.draw_query = Query(TransformComponent::Id(),
                                     SpriteComponent::Id());
        m_Queries.camera_query = Query(TransformComponent::Id(),
                                       CameraComponent::Id());
        m_Queries.scriptable_object_query = Query(ScriptComponent::Id());
        m_Queries.rigidbody_query = Query(RigidBody2DComponent::Id(), TransformComponent::Id());
}

}  // namespace Borek

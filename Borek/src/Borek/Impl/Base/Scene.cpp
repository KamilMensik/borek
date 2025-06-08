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
        RegisterComponent<TransformComponent>();
        RegisterComponent<TagComponent>();
        RegisterComponent<SpriteComponent>();
        RegisterComponent<CameraComponent>();
        RegisterComponent<ScriptComponent>();

        base_entity = m_World.get_archetype({ TransformComponent::Id(),
                                              TagComponent::Id() });
}

}  // namespace Borek

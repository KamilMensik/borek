// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <format>

#include <ECS/Archetype.h>
#include <ECS/World.h>

#include <box2d/box2d.h>
#include <Box2D/src/body.h>
#include <box2d/types.h>

#include "Include/Base/Scene.h"
#include "Include/Base/Components.h"

namespace Borek {

static ECS::ArchetypeId base_entity;

Entity Scene::NewEntity(const std::string& value)
{
        Entity e(m_World.entity(base_entity), this);

        TagComponent& tc = e.GetComponent<TagComponent>();
        e.GetComponent<IDComponent>().ecs_id = e.GetId();

        tc.value = std::string(value.empty() ? std::format("Entity {}", e.GetId()) : value);

        return e;
}

void Scene::DeleteEntity(Entity e)
{
        m_World.remove(e.m_Id);
}

Scene::Scene()
{
        using ECS::GetId;

        base_entity = m_World.get_archetype<IDComponent, TransformComponent, TagComponent>();

        m_RootNode = new Node(NewEntity("Root"));

        b2WorldDef wdef = b2DefaultWorldDef();
        wdef.gravity = b2Vec2(0.0f, -9.8f);
        m_PhysicsWorld = b2CreateWorld(&wdef);
}

Scene::~Scene()
{
        b2DestroyWorld(m_PhysicsWorld);
}

}  // namespace Borek

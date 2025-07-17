// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ECS/Component.h"
#include "Include/Base/Application.h"
#include "Include/Events/ApplicationEvents.h"
#include <box2d/box2d.h>

#include "Include/Base/Entity.h"
#include "Include/Base/Scene.h"

namespace Borek {

Entity::Entity(ECS::EntityId id, class Scene* scene)
        : m_Scene(scene), m_Id(id) {}

void* Entity::GetComponent(ECS::ComponentId cid)
{
        return m_Scene->m_World.get_component(m_Id, cid);
}

Entity& Entity::AddComponent(ECS::ComponentId cid)
{
        m_Scene->m_World.add_component(m_Id, cid);
        Application::SendEvent(new ComponentAddedEvent(cid, m_Id));
        return *this;
}

Entity& Entity::RemoveComponent(ECS::ComponentId cid)
{
        m_Scene->m_World.remove_component(m_Id, cid);
        Application::SendEvent(new ComponentRemovedEvent(cid, m_Id));
        return *this;
}

bool Entity::HasComponent(ECS::ComponentId cid)
{
        return m_Scene->m_World.has_component(m_Id, cid);
}

void Entity::Delete()
{
        m_Scene->DeleteEntity(*this);
}

TransformComponent& Entity::Transform()
{
        return GetComponent<TransformComponent>();
}
const char* Entity::GetName()
{
        return GetComponent<TagComponent>().value;
}

UUID Entity::GetUUID()
{
        return GetComponent<IDComponent>();
}

}  // namespace Borek

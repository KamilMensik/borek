// Copyright 2024-2025 <kamilekmensik@gmail.com>

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
        return *this;
}

Entity& Entity::RemoveComponent(ECS::ComponentId cid)
{
        m_Scene->m_World.remove_component(m_Id, cid);
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

}  // namespace Borek

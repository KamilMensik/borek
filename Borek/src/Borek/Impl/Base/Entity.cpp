// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Node.h"
#include "Include/Base/Entity.h"
#include "Include/Base/Scene.h"
#include "Include/Base/Application.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Debug/Log.h"
#include "Include/Debug/Assert.h"

#include "Include/Components/Text2DComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/TagComponent.h"
#include "Include/Components/IDComponent.h"

#include "Include/Engine/FZX/Body.h"
#include "Include/Events/ComponentEvents.h"

namespace Borek {

Entity::Entity(ECS::EntityId id)
        : m_Id(id) {}

void*
Entity::GetComponent(ECS::ComponentId cid)
{
        return Application::GetScene()->m_World->get_component(m_Id, cid);
}

Entity&
Entity::AddComponent(ECS::ComponentId cid)
{
        Application::GetScene()->m_World->add_component(m_Id, cid);
        Application::SendEvent<ComponentAddedEvent>(cid, m_Id);
        return *this;
}

Entity&
Entity::RemoveComponent(ECS::ComponentId cid)
{
        Application::GetScene()->m_World->remove_component(m_Id, cid);
        Application::SendEvent<ComponentRemovedEvent>(cid, m_Id);
        return *this;
}

bool
Entity::HasComponent(ECS::ComponentId cid)
{
        return Application::GetScene()->m_World->has_component(m_Id, cid);
}

void
Entity::Delete()
{
        Application::GetScene()->DeleteEntity(*this);
}

TransformComponent&
Entity::Transform()
{
        return GetComponent<TransformComponent>();
}

const TransformComponent
Entity::GlobalTransform()
{
        TransformComponent tc;

        Entity e = *this;
        while (!e.IsNil()) {
                tc += e.Transform();
                e = e.GetParent();
        }

        return tc;
}

const char*
Entity::GetName()
{
        return GetComponent<TagComponent>().value;
}

UUID
Entity::GetUUID()
{
        return GetComponent<IDComponent>();
}

bool
Entity::HasParent()
{
        return !GetParent().IsNil();
}

Entity
Entity::GetParent()
{
        return Application::GetScene()->GetEntityParent(*this);
}

bool
Entity::IsParentOf(Entity e)
{
        Entity parent = e.GetParent();
        while (!parent.IsNil()) {
                if (parent == *this)
                        return true;

                parent = parent.GetParent();
        }

        return false;
}

bool
Entity::HasChildren()
{
        return Application::GetScene()->HasEntityChildren(*this);
}

std::vector<uint32_t>*
Entity::GetChildren()
{
        return Application::GetScene()->GetEntityChildren(*this);
}

void
Entity::DeleteChildren()
{
        auto children = Application::GetScene()->GetEntityChildren(*this);
        if (!children)
                return;

        for (int i = children->size() - 1; i >= 0; i--) {
                Entity((*children)[i]).Delete();
        }
}

Entity
Entity::FindChild(const std::string& name)
{
        return Application::GetScene()->EntityFindFirstChild(name, *this);
}

NodeType
Entity::GetNodeType()
{
        return Application::GetScene()->GetEntityNodeType(*this);
}

glm::vec2
Entity::MoveAndCollide(float x, float y)
{
        BOREK_ENGINE_ASSERT(GetNodeType() == NodeType::DynamicBody, "Node is not dynamic body. Dont use .Move()");

        auto& body = GetComponent<FZX::BodyComponent>();

        auto res = Application::GetScene()->GetPhysicsWorld().MoveAndCollide(m_Id, body, x, y);
        Transform().position += res;
        return res;
}

bool
Entity::InitializeNode(NodeType type)
{
        if (type == GetNodeType())
                return false;

        DeinitializeNode();

        switch (type) {
        case NodeType::Node:
                break;
        case NodeType::Sprite:
                AddComponent<SpriteComponent>();
                break;
        case NodeType::StaticBody:
                AddComponent<FZX::BodyComponent>();
                GetComponent<FZX::BodyComponent>().body_type = FZX::BodyType::Static;
                break;
        case NodeType::DynamicBody:
                AddComponent<FZX::BodyComponent>();
                GetComponent<FZX::BodyComponent>().body_type = FZX::BodyType::Dynamic;
                break;
        case NodeType::Camera:
                AddComponent<CameraComponent>();
                break;
        case NodeType::Area:
                BOREK_ENGINE_ERROR("Area does not work yet");
                break;
        case NodeType::Text:
                AddComponent<Text2DComponent>();
                break;
        case NodeType::Tilemap:
                AddComponent<TilemapComponent>();
                break;
        case NodeType::SoundPlayer:
                AddComponent<SoundPlayerComponent>();
                break;
        }

        Application::GetScene()->m_EntityNodeTypes[m_Id] = type;
        
        return true;
}

void
Entity::DeinitializeNode()
{
        switch (GetNodeType()) {
        case NodeType::Node:
                break;
        case NodeType::Sprite:
                RemoveComponent<SpriteComponent>();
                break;
        case NodeType::StaticBody:
        case NodeType::DynamicBody:
                RemoveComponent<FZX::BodyComponent>();
                break;
        case NodeType::Camera:
                RemoveComponent<CameraComponent>();
                break;
        case NodeType::Area:
                BOREK_ENGINE_ERROR("Area does not work yet");
                break;
        case NodeType::Text:
                RemoveComponent<Text2DComponent>();
                break;
        case NodeType::Tilemap:
                RemoveComponent<TilemapComponent>();
                break;
        case NodeType::SoundPlayer:
                RemoveComponent<SoundPlayerComponent>();
                break;
        }
}

}  // namespace Borek

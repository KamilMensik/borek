// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string_view>

#include "Include/Base/Node.h"
#include "Include/Base/Entity.h"
#include "Include/Base/Scene.h"
#include "Include/Base/Application.h"
#include "Include/Base/TransformCache.h"
#include "Include/Debug/Assert.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/SceneTree.h"
#include "Include/Events/ComponentEvents.h"
#include "Include/Base/TransformCache.h"

#include "Include/Components/Text2DComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/ParticleComponent.h"

namespace Borek {

static inline Scene&
get_scene()
{
        return *Application::GetScene();
}

static inline SceneTree&
get_scene_tree()
{
        return Application::GetScene()->GetTree();
}

Entity::Entity(ECS::EntityId id)
        : m_Id(id) {}

void*
Entity::GetComponent(ECS::ComponentId cid)
{
        return get_scene().m_World->get_component(m_Id, cid);
}

const void*
Entity::GetComponentC(ECS::ComponentId cid) const
{
        return get_scene().m_World->get_component(m_Id, cid);
}

Entity&
Entity::AddComponent(ECS::ComponentId cid)
{
        get_scene().m_World->add_component(m_Id, cid);
        Application::SendEvent<ComponentAddedEvent>(cid, m_Id);
        return *this;
}

Entity&
Entity::RemoveComponent(ECS::ComponentId cid)
{
        get_scene().m_World->remove_component(m_Id, cid);
        Application::SendEvent<ComponentRemovedEvent>(cid, m_Id);
        return *this;
}

bool
Entity::HasComponent(ECS::ComponentId cid) const
{
        return get_scene().m_World->has_component(m_Id, cid);
}

void
Entity::Delete()
{
        get_scene().DeleteEntity(*this);
}

TransformComponent&
Entity::Transform()
{
        return GetComponent<TransformComponent>();
}

const TransformComponent
Entity::GlobalTransform() const
{
        return TransformCache::GetTransform(*this);
}

const char*
Entity::GetName() const
{
        return GetNameSym().Str().c_str();
}

Symbol
Entity::GetNameSym() const
{
        return get_scene_tree().GetEntityName(*this);
}

void
Entity::SetNameSym(Symbol& name)
{
        return get_scene_tree().SetEntityName(*this, name);
}

UUID
Entity::GetUUID() const
{
        return GetComponentC<IDComponent>();
}

bool
Entity::HasParent() const
{
        return !GetParent().IsNil();
}

Entity
Entity::GetParent() const
{
        return get_scene_tree().GetEntityParent(*this);
}

Entity
Entity::GetPrevSibling() const
{
        return get_scene_tree().GetEntityPrevSibling(*this);
}

Entity
Entity::GetNextSibling() const
{
        return get_scene_tree().GetEntityNextSibling(*this);
}

bool
Entity::IsParentOf(Entity e) const
{
        return get_scene_tree().IsEntityParentOf(*this, e);
}

bool
Entity::HasChildren() const
{
        return get_scene_tree().HasEntityChildren(*this);
}

const std::vector<Entity>*
Entity::GetChildren() const
{
        return get_scene_tree().GetEntityChildren(*this);
}

void
Entity::DeleteChildren()
{
        auto children = get_scene_tree().GetEntityChildren(*this);
        if (!children)
                return;

        for (int i = children->size() - 1; i >= 0; i--) {
                Entity e((*children)[i]);
                e.Delete();
        }
}

Entity
Entity::FindChild(Symbol name) const
{
        return get_scene_tree().EntityFindChild(name, *this);
}

NodeType
Entity::GetNodeType() const
{
        return get_scene_tree().GetEntityNodeType(*this);
}

uint32_t
Entity::ParentCount() const
{
        return get_scene_tree().GetEntityParentCount(*this);
}

uint64_t
Entity::GetRubyNode() const
{
        return get_scene_tree().GetEntityRubyNode(*this);
}

std::pair<FZX::Collision, glm::vec2>
Entity::MoveAndCollide(float x, float y)
{
        BOREK_ENGINE_ASSERT(GetNodeType() == NodeType::DynamicBody, "Node {} is not dynamic body. Dont use .Move()", GetName());

        auto& body = GetComponent<BodyComponent>();

        auto res = Application::GetScene()->GetPhysicsWorld().MoveAndCollide(m_Id, body, x, y);
        Transform().position += res.second;

        TransformCache::Invalidate(*this);

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
                AddComponent<BodyComponent>();
                GetComponent<BodyComponent>().body_type = FZX::BodyType::Static;
                GetComponent<BodyComponent>().collider_type = FZX::ColliderType::Rectangle;
                break;
        case NodeType::DynamicBody:
                AddComponent<BodyComponent>();
                GetComponent<BodyComponent>().body_type = FZX::BodyType::Dynamic;
                GetComponent<BodyComponent>().collider_type = FZX::ColliderType::Rectangle;
                break;
        case NodeType::Camera:
                AddComponent<CameraComponent>();
                break;
        case NodeType::Area:
                AddComponent<AreaComponent>();
                GetComponent<AreaComponent>().body_type = FZX::BodyType::Area;
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
        case NodeType::AnimatedSprite:
                AddComponent<AnimatedSpriteComponent>();
                break;
        case NodeType::ParticleEmmiter:
                AddComponent<ParticleComponent>();
                break;
        }

        get_scene_tree().SetEntityNodeType(*this, type);
        
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
                RemoveComponent<BodyComponent>();
                break;
        case NodeType::Camera:
                RemoveComponent<CameraComponent>();
                break;
        case NodeType::Area:
                RemoveComponent<AreaComponent>();
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
        case NodeType::AnimatedSprite:
                RemoveComponent<AnimatedSpriteComponent>();
                break;
        case NodeType::ParticleEmmiter:
                RemoveComponent<ParticleComponent>();
                break;
        }
}

std::string
Entity::PathTo(Entity other) const
{
        return get_scene_tree().PathTo(*this, other);
}

std::string
Entity::GetAbsolutePath() const
{
        Entity parent = GetParent();
        while (!parent.IsNil()) {
                parent = parent.GetParent();
        }
        return get_scene_tree().PathTo(get_scene_tree().GetRootEntity(), *this);
}

Entity
Entity::FindFromAbsolutePath(std::string_view path)
{
        return get_scene_tree().GetRootEntity().FindEntityByPath(path);
}

Entity
Entity::FindEntityByPath(std::string_view path) const
{
        return get_scene_tree().FindEntityByPath(*this, path);
}

}  // namespace Borek

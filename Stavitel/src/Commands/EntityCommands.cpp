// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Commands/EntityCommands.h"
#include "Include/Base/Application.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/ParticleComponent.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/Text2DComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/ValueComponent.h"
#include "Include/Debug/Log.h"
#include "Panels/PanelEvents.h"

namespace Borek {

static RelatedEntityPath
get_related_entity_path(Entity e)
{
        Entity prev_sibling = e.GetNextSibling();
        Entity parent = e.GetParent();

        RelatedEntityPath res;

        if (prev_sibling.IsNil()) {
                res.path = parent.GetAbsolutePath();
                res.is_parent = true;
        } else {
                res.is_parent = false;
                res.path = prev_sibling.GetAbsolutePath();
        }

        return res;
}

static void
move_entity_to_related_entity(Entity e, RelatedEntityPath rel)
{
        SceneTree& tree = Application::GetScene()->GetTree();
        Entity related_entity = Entity::FindFromAbsolutePath(rel.path.Str());
        if (rel.is_parent) {
                tree.EntityAppendChild(e, related_entity);
        } else {
                tree.EntityPrepend(e, related_entity);
        }
}

template <class T>
static void
import_component(Entity e, void* data)
{
        e.AddComponent<T>();
        auto& val = e.GetComponent<T>();
        val = std::move(*RCAST<T*>(data));
        delete RCAST<T*>(data);
}

template <class T>
static void
export_component(Entity e, void** out)
{
        T* component = new T;
        *component = std::move(e.GetComponent<T>());
        *out = component;
}


RemoveEntityCommand::RemoveEntityCommand(Entity e)
{
        m_PathToEntity = e.GetAbsolutePath();
        m_RelatedEntityPath = get_related_entity_path(e);
}

void
RemoveEntityCommand::Undo()
{
        Entity e = Application::GetScene()->ImportEntity(m_Export);
        move_entity_to_related_entity(e, m_RelatedEntityPath);
}

void
RemoveEntityCommand::Redo()
{
        Entity e = Entity::FindFromAbsolutePath(m_PathToEntity.Str());
        m_Export = Application::GetScene()->ExportEntity(e);
        Application::SendEventImmediate<ChangeEntityEvent>(Entity());
}

RenameEntityCommand::RenameEntityCommand(Entity e, const Symbol& new_name)
{
        m_PathToEntity = e.GetAbsolutePath();
        m_OldName = e.GetNameSym();
        m_NewName = new_name;
}

void
RenameEntityCommand::Undo()
{
        Entity::FindFromAbsolutePath(m_PathToEntity.Str()).SetNameSym(m_OldName);
}

void
RenameEntityCommand::Redo()
{
        BOREK_ENGINE_INFO("Renaming entity {} to: {}", m_PathToEntity.Str(), m_NewName.Str());
        Entity::FindFromAbsolutePath(m_PathToEntity.Str()).SetNameSym(m_NewName);
}

MoveEntityCommand::MoveEntityCommand(Entity e, const RelatedEntity& dest)
{
        m_OldPathToEntity = e.GetAbsolutePath();
        m_PathToOldRelatedEntity = get_related_entity_path(e);
        m_PathToNewRelatedEntity.path = dest.entity.GetAbsolutePath();
        m_PathToNewRelatedEntity.is_parent = dest.is_parent;
}

void
MoveEntityCommand::Undo()
{
        Entity e = Entity::FindFromAbsolutePath(m_NewPathToEntity.Str());
        move_entity_to_related_entity(e, m_PathToOldRelatedEntity);
}

void
MoveEntityCommand::Redo()
{
        Entity e = Entity::FindFromAbsolutePath(m_OldPathToEntity.Str());
        move_entity_to_related_entity(e, m_PathToNewRelatedEntity);
        m_NewPathToEntity = e.GetAbsolutePath();
}

ChangeEntityNodeTypeCommand::ChangeEntityNodeTypeCommand(Entity e, NodeType new_node_type)
{
        m_OldNodeType = e.GetNodeType();
        m_NewNodeType = new_node_type;
        m_PathToEntity = e.GetAbsolutePath();
}

void
ChangeEntityNodeTypeCommand::Undo()
{
        Entity e = Entity::FindFromAbsolutePath(m_PathToEntity.Str());
        e.DeinitializeNode();

        switch (m_OldNodeType) {
        case NodeType::Node:
                break;
        case NodeType::Sprite:
                import_component<SpriteComponent>(e, m_Data);
                break;
        case NodeType::StaticBody:
        case NodeType::DynamicBody:
                import_component<BodyComponent>(e, m_Data);
                break;
        case NodeType::Camera:
                import_component<CameraComponent>(e, m_Data);
                break;
        case NodeType::Area:
                import_component<AreaComponent>(e, m_Data);
                break;
        case NodeType::Text:
                import_component<Text2DComponent>(e, m_Data);
                break;
        case NodeType::Tilemap:
                import_component<TilemapComponent>(e, m_Data);
                break;
        case NodeType::SoundPlayer:
                import_component<SoundPlayerComponent>(e, m_Data);
                break;
        case NodeType::AnimatedSprite:
                import_component<AnimatedSpriteComponent>(e, m_Data);
                break;
        case NodeType::ParticleEmmiter:
                import_component<ParticleComponent>(e, m_Data);
                break;
        }

        Application::GetScene()->GetTree().SetEntityNodeType(e, m_OldNodeType);
}

void
ChangeEntityNodeTypeCommand::Redo()
{
        Entity e = Entity::FindFromAbsolutePath(m_PathToEntity.Str());

        switch (m_OldNodeType) {
        case NodeType::Node:
                break;
        case NodeType::Sprite:
                export_component<SpriteComponent>(e, &m_Data);
                break;
        case NodeType::StaticBody:
        case NodeType::DynamicBody:
                export_component<BodyComponent>(e, &m_Data);
                break;
        case NodeType::Camera:
                export_component<CameraComponent>(e, &m_Data);
                break;
        case NodeType::Area:
                export_component<AreaComponent>(e, &m_Data);
                break;
        case NodeType::Text:
                export_component<Text2DComponent>(e, &m_Data);
                break;
        case NodeType::Tilemap:
                export_component<TilemapComponent>(e, &m_Data);
                break;
        case NodeType::SoundPlayer:
                export_component<SoundPlayerComponent>(e, &m_Data);
                break;
        case NodeType::AnimatedSprite:
                export_component<AnimatedSpriteComponent>(e, &m_Data);
                break;
        case NodeType::ParticleEmmiter:
                export_component<ParticleComponent>(e, &m_Data);
                break;
        }

        e.InitializeNode(m_NewNodeType);
}

CreateEntityCommand::CreateEntityCommand(
        const Symbol& name, NodeType type, Entity parent)
        : m_Name(name), m_NodeType(type), m_ParentPath(parent.GetAbsolutePath())
{
}

void
CreateEntityCommand::Undo()
{
        m_CreatedEntity.Delete();
}

void
CreateEntityCommand::Redo()
{
        m_CreatedEntity = Application::GetScene()->NewEntity(
                m_Name,
                m_NodeType,
                Entity::FindFromAbsolutePath(m_ParentPath.Str()));
}

CreatePrefabCommand::CreatePrefabCommand(
        const Symbol& prefab_path, Entity parent, const glm::vec2& pos)
        : m_PrefabPath(prefab_path),
        m_ParentPath(parent.GetAbsolutePath()),
        m_Pos(pos)
{
}

void
CreatePrefabCommand::Undo()
{
        m_CreatedEntity.Delete();
}

void
CreatePrefabCommand::Redo()
{
        m_CreatedEntity = Application::GetScene()->CreateFromPrefab(
                m_PrefabPath.Str(),
                Entity::FindFromAbsolutePath(m_ParentPath.Str()));
        m_CreatedEntity.Transform().position = m_Pos;
}

}  // namespace Borek

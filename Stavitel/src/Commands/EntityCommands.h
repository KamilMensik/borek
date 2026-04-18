// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <Borek/Include/Base/Node.h>
#include <Borek/Include/Base/Scene.h>
#include <Borek/Include/Base/TransformCache.h>
#include <Borek/Include/Components/TransformComponent.h>
#include <Borek/Include/Base/Symbol.h>
#include <Borek/Include/Base/Entity.h>

#include <ECS/ArchetypeExport.h>

#include "Commands/ICommand.h"
#include <concepts>

namespace Borek {

struct RelatedEntity {
        Entity entity;
        bool is_parent;
};

struct RelatedEntityPath {
        Symbol path;
        bool is_parent;
};

class RemoveEntityCommand : public ICommand {
public:
        RemoveEntityCommand(Entity e);

        void
        Undo() override;

        void
        Redo() override;

private:
        SceneExport m_Export;
        Symbol m_PathToEntity;
        RelatedEntityPath m_RelatedEntityPath;
};

class RenameEntityCommand : public ICommand {
public:
        RenameEntityCommand(Entity e, const Symbol& new_name);

        void
        Undo() override;

        void
        Redo() override;

private:
        Symbol m_PathToEntity;
        Symbol m_OldName;
        Symbol m_NewName;
};

class MoveEntityCommand : public ICommand {
public:
        MoveEntityCommand(Entity e, const RelatedEntity& dest);

        void
        Undo() override;

        void
        Redo() override;

private:
        Symbol m_OldPathToEntity;
        Symbol m_NewPathToEntity;
        RelatedEntityPath m_PathToOldRelatedEntity;
        RelatedEntityPath m_PathToNewRelatedEntity;
};

class ChangeEntityNodeTypeCommand : public ICommand {
public:
        ChangeEntityNodeTypeCommand(Entity e, NodeType new_node_type);

        void
        Undo() override;

        void
        Redo() override;

private:
        Symbol m_PathToEntity;
        NodeType m_OldNodeType;
        NodeType m_NewNodeType;
        void* m_Data;
};

template <class T> requires (!std::derived_from<T, TransformComponent>)
class ModifyEntityComponentCommand : public ICommand {
public:
        ModifyEntityComponentCommand(Entity e, T& new_value)
        {
                m_OldValue = e.GetComponent<T>();
                m_NewValue = new_value;
                m_PathToEntity = e.GetAbsolutePath();
        }

        void
        Undo() override
        {
                Entity::FindFromAbsolutePath(m_PathToEntity.Str())
                        .GetComponent<T>() = m_OldValue;
        }

        void
        Redo() override
        {
                Entity::FindFromAbsolutePath(m_PathToEntity.Str())
                        .GetComponent<T>() = m_NewValue;

        }

private:
        T m_OldValue;
        T m_NewValue;
        Symbol m_PathToEntity;
};

class ModifyEntityTransformCommand : public ICommand {
public:
        ModifyEntityTransformCommand(Entity e, TransformComponent& new_value)
        {
                m_OldValue = e.GetComponent<TransformComponent>();
                m_NewValue = new_value;
                m_PathToEntity = e.GetAbsolutePath();
        }

        void
        Undo() override
        {
                Entity e = Entity::FindFromAbsolutePath(m_PathToEntity.Str());
                e.GetComponent<TransformComponent>() = m_OldValue;
                TransformCache::Invalidate(e);
        }

        void
        Redo() override
        {
                Entity e = Entity::FindFromAbsolutePath(m_PathToEntity.Str());
                e.GetComponent<TransformComponent>() = m_NewValue;
                TransformCache::Invalidate(e);
        }

private:
        TransformComponent m_OldValue;
        TransformComponent m_NewValue;
        Symbol m_PathToEntity;
};

class CreateEntityCommand : public ICommand {
public:
        CreateEntityCommand(const Symbol& name, NodeType type, Entity parent);

        void
        Undo() override;

        void
        Redo() override;

private:
        Symbol m_Name;
        NodeType m_NodeType;
        Symbol m_ParentPath;
        Entity m_CreatedEntity;
};

class CreatePrefabCommand : public ICommand {
public:
        CreatePrefabCommand(const Symbol& prefab_path, Entity parent,
                            const glm::vec2& pos = glm::vec2(0));

        void
        Undo() override;

        void
        Redo() override;

private:
        Symbol m_PrefabPath;
        Symbol m_ParentPath;
        Entity m_CreatedEntity;
        glm::vec2 m_Pos;
};

}  // namespace Borek

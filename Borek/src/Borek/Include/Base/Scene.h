// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>
#include <vector>

#include <ECS/World.h>
#include <ECS/Component.h>
#include <ECS/ArchetypeExport.h>

#include "Include/Base/Entity.h"
#include "Include/Base/Node.h"
#include "Include/Core.h"
#include "Include/Engine/FZX/FACD.h"
#include "Include/Engine/SceneTree.h"

#pragma once

namespace Borek {

struct SceneExportItem : public ECS::ArchetypeExport {
        Symbol entity_name;
        NodeType entity_node_type;
        int m;
};

using SceneExport = std::vector<SceneExportItem>;

class Scene {
friend class Entity;
friend class SceneSerializer;
friend class Application;
friend class EntityInitializer;
public:
        Scene();
        ~Scene();

        void
        Init();

        void
        Initialize();

        void
        Uninitialize();

        Entity
        NewEntity(Symbol tag = Symbol(), NodeType type = NodeType::Node,
                  Entity parent = Entity());

        Entity
        CreateFromPrefab(std::string_view path, Entity parent = Entity());

        void
        DeleteEntity(Entity e);

        uint32_t
        GetEntityCount();

        FZX::FACD&
        GetPhysicsWorld();

        SceneTree&
        GetTree();

        const std::filesystem::path&
        GetPath() const;

        void
        SetPath(const std::filesystem::path& path, bool force_extension = true);

        SceneExport
        ExportEntity(Entity e);

        Entity
        ImportEntity(SceneExport& e);

        Entity
        GetLastCreatedEntity();

public:
        FZX::FACD m_PhysicsWorld;
        SceneTree m_SceneTree;
        std::filesystem::path m_Path;
        Uniq<ECS::World> m_World;
        Entity m_LastCreatedEntity;
        bool m_Initialized;

        template <class T>
        inline T&
        GetComponent(Entity e)
        {
                return *RCAST<T*>(m_World->get_component(e, ECS::GetId<T>()));
        }
};

}  // namespace Borek

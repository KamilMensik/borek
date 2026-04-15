#include <format>

#include <ECS/Archetype.h>
#include <ECS/World.h>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Base/Scene.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/TagComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Components/ZIndexComponent.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Base/Application.h"
#include "Include/Engine/EntityInitializer.h"
#include "Include/Engine/EntityUninitializer.h"

namespace Borek {

static ECS::Archetype base_entity;

Scene::Scene() : m_Initialized(false)
{
        base_entity = ECS::Archetype::Get<
                IDComponent, TransformComponent, TagComponent, ZIndexComponent
        >();
}

Scene::~Scene()
{
        std::move(m_SceneTree).Destroy([this](Entity e){
                m_World->remove(e);
        });
}

void
Scene::Init()
{
        if (m_World)
                return;

        m_World = NewUniq<ECS::World>();
        m_SceneTree.SetRootEntity(NewEntity("Root"));
}

void
Scene::Initialize()
{
        if (m_Initialized == true)
                return;

        m_SceneTree.TraverseScene(EntityInitializer::InitializeBegin,
                                  EntityInitializer::InitializeEnd);
        m_Initialized = true;
}

void
Scene::Uninitialize()
{
        if (m_Initialized == false)
                return;

        m_SceneTree.TraverseScene(EntityUninitializer::UninitializeBegin,
                                  EntityUninitializer::UninitializeEnd);
        m_Initialized = false;
}

Entity
Scene::NewEntity(Symbol value, NodeType type, Entity parent)
{
        Entity e(m_World->entity(base_entity));
        e.GetComponent<IDComponent>().ecs_id = e;
        if (value.IsNil()) {
                value = std::format("Entity {}", e.GetId());
        }
        m_SceneTree.AddEntity(e, value, type, parent);
        e.InitializeNode(type);
        m_LastCreatedEntity = e;

        return e;
}

void
Scene::DeleteEntity(Entity e)
{
        if (Application::IsPlaying()) {
                EntityUninitializer::UninitializeEntity(e);
        }
        e.DeleteChildren();
        m_SceneTree.RemoveEntity(e);
        m_World->remove(e.m_Id);
}

uint32_t
Scene::GetEntityCount()
{
        return m_SceneTree.GetEntityCount();
}


FZX::FACD&
Scene::GetPhysicsWorld()
{
        return m_PhysicsWorld;
}

SceneTree&
Scene::GetTree()
{
        return m_SceneTree;
}

const std::filesystem::path&
Scene::GetPath() const
{
        return m_Path;
}

void
Scene::SetPath(const std::filesystem::path& path, bool force_extension)
{
        m_Path = path;
        if (force_extension)
                m_Path.replace_extension("scn");
}

SceneExport
Scene::ExportEntity(Entity e)
{
        SceneExport exp;
        m_SceneTree.TraverseScene(e, [&exp, this](Entity e) {
                exp.emplace_back(m_World->get_export(e),
                                 e.GetNameSym(), e.GetNodeType(), 1);
        }, [&exp](Entity e) {
                exp.back().m -= 1;
        });

        m_SceneTree.DestroyEntity(e);

        return exp;
}

Entity
Scene::ImportEntity(SceneExport& exp)
{
        if (exp.size() == 1) {
                Entity e(m_World->entity(exp.back()));
                e.GetComponent<IDComponent>().ecs_id = e;
                m_SceneTree.SetEntityName(e, exp.back().entity_name);
                m_SceneTree.SetEntityNodeType(e, exp.back().entity_node_type);
                return e;
        }

        std::vector<Entity> entity_stack;
        for (auto& item : exp) {
                Entity e(m_World->entity(item));
                e.GetComponent<IDComponent>().ecs_id = e;
                m_SceneTree.SetEntityName(e, item.entity_name);
                m_SceneTree.SetEntityNodeType(e, item.entity_node_type);

                if (!entity_stack.empty()) {
                        m_SceneTree.EntityAppendChild(e, entity_stack.back());
                }

                if (item.m > 0) {
                        entity_stack.emplace_back(e);
                        continue;
                }

                for (int i = item.m; i < 0; i++) {
                        if (entity_stack.size() == 1)
                                break;

                        entity_stack.pop_back();
                }
        }

        return entity_stack.back();
}

Entity
Scene::GetLastCreatedEntity()
{
        return m_LastCreatedEntity;
}

Entity
Scene::CreateFromPrefab(std::string_view path, Entity parent)
{
        Entity e = NewEntity().AddComponent<PrefabComponent>();
        auto& prefab = e.GetComponent<PrefabComponent>();
        prefab.scene = AssetManager::Get<SceneAsset>(path);
        prefab.Update(e);

        auto begin = path.find_last_of('/');
        auto end = path.find_last_of('.');
        Symbol name = path.substr(begin + 1, end - begin - 1);

        if (parent.IsNil())
                parent = m_SceneTree.GetRootEntity();

        m_SceneTree.EntityAppendChild(e, parent);
        e.SetNameSym(name);
        return e;
}

}  // namespace Borek

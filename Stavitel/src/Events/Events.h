// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

#include <Borek/Include/Events/CustomEvent.h>
#include <Borek/Include/Base/Entity.h>

namespace Borek {

class ScenePanelSelectedEvent : public CustomEvent<ScenePanelSelectedEvent> {
public:
        ScenePanelSelectedEvent(Entity e) : m_Entity(e) {}
        inline Entity GetEntity() { return m_Entity; }

private:
        Entity m_Entity;
};

class AssetPanelSelectedEvent : public CustomEvent<AssetPanelSelectedEvent> {
public:
        AssetPanelSelectedEvent(const std::string& path) : m_AssetPath(path) {}
        inline const std::string& GetAssetPath() { return m_AssetPath; }

private:
        std::string m_AssetPath;
};

class ChangeSceneEvent : public CustomEvent<ChangeSceneEvent> {
public:
        ChangeSceneEvent(const std::string& scene_path)
                : m_ScenePath(scene_path) {}

        inline std::string& GetScenePath() { return m_ScenePath; }

private:
        std::string m_ScenePath;
};

class AddComponentEvent : public CustomEvent<AddComponentEvent> {
public:
        AddComponentEvent(uint32_t id, uint32_t entity_id) : m_EntityId(entity_id), m_Id(id) {}

        inline uint32_t GetEntityId() { return m_EntityId; }
        inline uint32_t GetId() { return m_Id; }

private:
        uint32_t m_EntityId;
        uint32_t m_Id;
};

class RemoveComponentEvent : public CustomEvent<RemoveComponentEvent> {
public:
        RemoveComponentEvent(uint32_t id, uint32_t entity_id) : m_EntityId(entity_id), m_Id(id) {}

        inline uint32_t GetEntityId() { return m_EntityId; }
        inline uint32_t GetId() { return m_Id; }

private:
        uint32_t m_EntityId;
        uint32_t m_Id;
};

class RemoveEntityEvent : public CustomEvent<RemoveEntityEvent> {
public:
        RemoveEntityEvent(uint32_t id) : m_EntityId(id) {}

        inline uint32_t
        GetEntityId() { return m_EntityId; }

private:
        uint32_t m_EntityId;
};

}  // namespace Borek

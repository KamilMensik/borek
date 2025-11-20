// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

#include <Borek/Include/Events/Event.h>
#include <Borek/Include/Base/Entity.h>

namespace Borek {

class ScenePanelSelectedEvent : public Event {
_BASE_EVENT_HEADER(ScenePanelSelectedEvent)
public:
        ScenePanelSelectedEvent(Entity e) : m_Entity(e) {}
        inline Entity GetEntity() { return m_Entity; }

private:
        Entity m_Entity;
};

class AssetPanelSelectedEvent : public Event {
_BASE_EVENT_HEADER(AssetPanelSelectedEvent)
public:
        AssetPanelSelectedEvent(const std::string& path) : m_AssetPath(path) {}
        inline const std::string& GetAssetPath() { return m_AssetPath; }

private:
        std::string m_AssetPath;
};

class ChangeSceneEvent : public Event {
_BASE_EVENT_HEADER(ChangeSceneEvent)
public:
        ChangeSceneEvent(const std::string& scene_path)
                : m_ScenePath(scene_path) {}

        inline std::string& GetScenePath() { return m_ScenePath; }

private:
        std::string m_ScenePath;
};

class AddComponentEvent : public Event {
_BASE_EVENT_HEADER(AddComponentEvent)
public:
        AddComponentEvent(uint32_t id, Entity entity) : m_Entity(entity), m_Id(id) {}

        inline Entity GetEntity() { return m_Entity; }
        inline uint32_t GetId() { return m_Id; }

private:
        Entity m_Entity;
        uint32_t m_Id;
};

class RemoveComponentEvent : public Event {
_BASE_EVENT_HEADER(RemoveComponentEvent)
public:
        RemoveComponentEvent(uint32_t id, uint32_t entity) : m_Entity(entity), m_Id(id) {}

        inline Entity GetEntity() { return m_Entity; }
        inline uint32_t GetId() { return m_Id; }

private:
        Entity m_Entity;
        uint32_t m_Id;
};

class RemoveEntityEvent : public Event {
_BASE_EVENT_HEADER(RemoveEntityEvent)
public:
        RemoveEntityEvent(Entity e) : m_Entity(e) {}

        inline Entity
        GetEntity() { return m_Entity; }

private:
        Entity m_Entity;
};

}  // namespace Borek

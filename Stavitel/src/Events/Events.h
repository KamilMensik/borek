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

class ChangeSceneEvent : public CustomEvent<ChangeSceneEvent> {
public:
        ChangeSceneEvent(const std::string& scene_path)
                : m_ScenePath(scene_path) {}

        inline std::string& GetScenePath() { return m_ScenePath; }

private:
        std::string m_ScenePath;
};

}  // namespace Borek

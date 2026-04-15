// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string_view>
#include <string>

#include "Include/Events/Event.h"

namespace Borek {

class SceneChangedEvent : public Event {
_BASE_EVENT_HEADER(SceneChangedEvent);
public:
        SceneChangedEvent() {}
};

class ChangeSceneEvent : public Event {
_BASE_EVENT_HEADER(ChangeSceneEvent)
public:
        ChangeSceneEvent(std::string_view scene_path)
                : m_ScenePath(scene_path) {}

        inline std::string& GetScenePath() { return m_ScenePath; }

private:
        std::string m_ScenePath;
};

}  // namespace Borek

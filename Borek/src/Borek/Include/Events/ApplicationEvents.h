// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Scene.h"
#include "Include/Events/CustomEvent.h"
#include "Include/Events/Event.h"

namespace Borek {

class TickEvent : public Event {
        _EVENT_CLASS_TYPE(Tick);
        _EVENT_CLASS_CATEGORY(kApplication);

        TickEvent() {}
};

class UpdateEvent : public Event {
        _EVENT_CLASS_TYPE(Update);
        _EVENT_CLASS_CATEGORY(kApplication);

        UpdateEvent() {}
};

class RenderEvent : public Event {
        _EVENT_CLASS_TYPE(Render);
        _EVENT_CLASS_CATEGORY(kApplication);

        RenderEvent() {}
};

class SceneChangedEvent : public CustomEvent<SceneChangedEvent> {
public: 
        SceneChangedEvent() {}
        Ref<Scene> GetScene();
};

}  // namespace Borek

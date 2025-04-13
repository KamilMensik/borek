// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "./Event.h"

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

}  // namespace Borek

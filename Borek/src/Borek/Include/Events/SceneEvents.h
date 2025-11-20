// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/Event.h"

namespace Borek {

class SceneChangedEvent : public Event {
_BASE_EVENT_HEADER(SceneChangedEvent);
public:
        SceneChangedEvent() {}
};

}  // namespace Borek

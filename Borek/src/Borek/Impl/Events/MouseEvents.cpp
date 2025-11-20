// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/MouseEvents.h"
#include "Include/Events/Event.h"

namespace Borek {

_BASE_EVENT_IMPL(MouseMovedEvent, Event);
_BASE_EVENT_IMPL(MouseButtonEvent, Event);
_BASE_EVENT_IMPL(MouseScrolledEvent, Event);

}  // namespace Borek

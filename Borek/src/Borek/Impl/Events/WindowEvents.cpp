// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/WindowEvents.h"

namespace Borek {

_BASE_EVENT_IMPL(WindowResizeEvent, Event)
_BASE_EVENT_IMPL(WindowCloseEvent, Event)
_BASE_EVENT_IMPL(WindowFocusEvent, Event)
_BASE_EVENT_IMPL(WindowLostFocusEvent, Event)
_BASE_EVENT_IMPL(WindowMoveEvent, Event)

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/ResourceEvents.h"
#include "Include/Events/Event.h"

namespace Borek {

_BASE_EVENT_IMPL(ResourceRenamedEvent, Event);
_BASE_EVENT_IMPL(ResourceDeletedEvent, Event);

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"

namespace Borek {

_BASE_EVENT_IMPL(ScenePanelSelectedEvent, Event);
_BASE_EVENT_IMPL(AssetPanelSelectedEvent, Event);
_BASE_EVENT_IMPL(ChangeSceneEvent, Event);
_BASE_EVENT_IMPL(AddComponentEvent, Event);
_BASE_EVENT_IMPL(RemoveComponentEvent, Event);
_BASE_EVENT_IMPL(RemoveEntityEvent, Event);

}  // namespace Borek

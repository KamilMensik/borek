// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/ApplicationEvents.h"
#include "Include/Base/Application.h"

namespace Borek {

Ref<Scene> SceneChangedEvent::GetScene()
{
        return Application::GetScene();
}

}  // namespace Borek

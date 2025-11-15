// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/ZIndexAssigner.h"
#include "Include/Base/Application.h"
#include "Include/Base/Scene.h"
#include "Include/Components/ZIndexComponent.h"

namespace Borek {

void
ZIndexAssigner::Assign()
{
        Scene& scene = *Application::GetScene();

        s_Current = scene.GetEntityCount() * 0.00006f;
        scene.TraverseScene(AssignHelper);
        s_Current = scene.GetEntityCount() * 0.00006f;
}

void
ZIndexAssigner::AssignHelper(Entity e)
{
        e.GetComponent<ZIndexComponent>().value = s_Current;
        s_Current -= 0.00006f;
}

float ZIndexAssigner::s_Current = 0;

}  // namespace Borek

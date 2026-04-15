// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Debug/Log.h"

namespace Borek {

using Cam = CameraComponent;
using Trans = TransformComponent;


Cam::CameraComponent() : camera(),  zoom(1), is_active(false)
{
}

const glm::mat4&
Cam::GetViewProjectionMatrix(const Trans& transform)
{
        Recalculate(transform);

        return camera.GetViewProjectionMatrix();
}

bool
Cam::OnWindowResized(WindowResizeEvent& ev)
{
        width = ev.GetWidth();
        height = ev.GetHeight();
        camera.SetProjection(-zoom * (width >> 1), zoom * (width >> 1),
                             -zoom * (height >> 1), zoom * (height >> 1));
        return false;
}

void
Cam::Recalculate(const Trans& transform)
{
        camera.SetProjection(-zoom * (width >> 1), zoom * (width >> 1),
                             -zoom * (height >> 1), zoom * (height >> 1));
        camera.Recalculate(glm::vec3(transform.position, 0.0f),
                           glm::vec3(0, 0, transform.rotation));
}


}  // namespace Borek

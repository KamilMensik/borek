// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"

namespace Borek {

using Cam = CameraComponent;
using Trans = TransformComponent;

Cam::CameraComponent(float aspect_ratio, float zoom)
        : aspect_ratio(aspect_ratio), zoom(zoom),
        camera(-aspect_ratio * zoom, aspect_ratio * zoom, -zoom, zoom),
        is_active(false)
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
        aspect_ratio = ((float)ev.GetWidth() / ev.GetHeight());
        camera.SetProjection(-aspect_ratio * zoom, aspect_ratio * zoom,
                               -zoom, zoom);
        return false;
}

void
Cam::Recalculate(const Trans& transform)
{
        camera.SetProjection(-640 * zoom, 640 * zoom,
                             -360 * zoom, 360 * zoom);
        camera.Recalculate(glm::vec3(transform.position, 0.0f), glm::vec3(0, 0, transform.rotation));
}

}  // namespace Borek

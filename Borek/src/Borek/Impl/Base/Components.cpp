// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Components.h"
#include "Include/Debug/Log.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

namespace Borek {

using Cam = CameraComponent;
using Trans = TransformComponent;

constexpr static glm::vec3 FromPixels(const glm::vec3& pixels) {
        return pixels / 400.0f;
}

Trans& Trans::Move(float x, float y)
{
        position.x += x;
        position.y += y;
        return *this;
}

Trans& Trans::Move(float x, float y, float z)
{
        position += glm::vec3(x,y,z);
        return *this;
}

Trans& Trans::Move(const glm::vec2& xy)
{
        position += glm::vec3(xy, 0.0f);
        return *this;
}

Trans& Trans::Move(const glm::vec3& xyz)
{
        position += xyz;
        return *this;
}

glm::mat4 Trans::GetTransformMat() const
{
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

        if (rotation.x != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.x),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (rotation.y != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.y),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotation.z != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.z),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        return glm::scale(transform, scale);
}

glm::mat4 Trans::GetPixelTransformMat() const
{
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), FromPixels(position));

        if (rotation.x != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.x),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (rotation.y != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.y),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotation.z != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.z),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        return glm::scale(transform, FromPixels(scale));
}


Cam::CameraComponent(float aspect_ratio, float zoom)
        : aspect_ratio(aspect_ratio), zoom(zoom),
        camera(-aspect_ratio * zoom, aspect_ratio * zoom, -zoom, zoom)
{
        BOREK_LOG_INFO("Set aspect ratio to: {}", aspect_ratio);
}

const glm::mat4& Cam::GetViewProjectionMatrix(const Trans& transform)
{
        Recalculate(transform);

        return camera.GetViewProjectionMatrix();
}

bool Cam::OnWindowResized(WindowResizeEvent& ev)
{
        aspect_ratio = ((float)ev.GetWidth() / ev.GetHeight());
        camera.SetProjection(-aspect_ratio * zoom, aspect_ratio * zoom,
                               -zoom, zoom);
        return false;
}

void Cam::Recalculate(const Trans& transform)
{
        camera.SetProjection(-aspect_ratio * zoom, aspect_ratio * zoom,
                             -zoom, zoom);
        camera.Recalculate(transform.position, transform.rotation);
}

}  // namespace Borek

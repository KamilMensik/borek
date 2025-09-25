// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Objects/Camera2D.h"
#include "Include/Events/EventCaller.h"
#include "Include/Core.h"

namespace Borek {

Camera2D::Camera2D(float aspect_ratio)
        : m_AspectRatio(aspect_ratio),
        m_Camera(-640 * m_Zoom, 640 * m_Zoom, -360 * m_Zoom, 360 * m_Zoom)
{
        Recalculate();
}

Camera2D& Camera2D::SetPos(glm::vec2 pos)
{
        m_Position = pos;
        m_Camera.Invalidate();
        return *this;
}

Camera2D& Camera2D::Move(glm::vec2 pos)
{
        m_Position += pos;
        m_Camera.Invalidate();
        return *this;
}

Camera2D& Camera2D::Rotate(float rotation)
{
        m_Rotation = rotation;
        m_Camera.Invalidate();
        return *this;
}

Camera2D& Camera2D::SetZoom(float zoom)
{
        m_Zoom = zoom;
        m_Camera.SetProjection(-640 * m_Zoom, 640 * m_Zoom,
                               -360 * m_Zoom, 360 * m_Zoom);
        return *this;
}

void Camera2D::OnEvent(Event& ev)
{
        EventCaller ec(ev);
        ec.TryCall<WindowResizeEvent>(EVENT_FN(Camera2D::OnWindowResized));
}

const glm::mat4& Camera2D::GetViewProjectionMatrix()
{
        if (!m_Camera.IsValid())
                Recalculate();

        return m_Camera.GetViewProjectionMatrix();
}

bool Camera2D::OnWindowResized(WindowResizeEvent& ev)
{
        m_AspectRatio = ((float)ev.GetWidth() / ev.GetHeight());
        float size_x = ev.GetWidth() / 2.0;
        float size_y = ev.GetHeight() / 2.0;
        m_Camera.SetProjection(-size_x * m_Zoom, size_x * m_Zoom,
                               -size_y * m_Zoom, size_y * m_Zoom);
        return false;
}

void Camera2D::Recalculate()
{
        m_Camera.Recalculate(glm::vec3(m_Position, 0.0f),
                             glm::vec3(0.0f, 0.0f, m_Rotation));
}

}  // namespace Borek

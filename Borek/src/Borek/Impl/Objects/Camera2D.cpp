// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Objects/Camera2D.h"
#include "Include/Events/EventCaller.h"

namespace Borek {

Camera2D::Camera2D(float aspect_ratio)
        : m_AspectRatio(aspect_ratio),
        m_Camera(-aspect_ratio * m_Zoom, aspect_ratio * m_Zoom, -m_Zoom, m_Zoom)
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
        m_Camera.SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom,
                               -m_Zoom, m_Zoom);
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
        m_Camera.SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom,
                               -m_Zoom, m_Zoom);
        return false;
}

void Camera2D::Recalculate()
{
        m_Camera.Recalculate(glm::vec3(m_Position, 0.0f),
                             glm::vec3(0.0f, 0.0f, m_Rotation));
}

}  // namespace Borek

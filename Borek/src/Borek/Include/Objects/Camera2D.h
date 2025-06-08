// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Include/Events/WindowEvents.h"
#include "Include/Graphics/Camera.h"

namespace Borek {

class Camera2D {
public:
        Camera2D(float aspect_ratio);
        Camera2D& SetPos(glm::vec2 pos);
        Camera2D& Move(glm::vec2 pos);
        Camera2D& Rotate(float rotation);
        Camera2D& SetZoom(float zoom);

        void OnEvent(Event& ev);

        const glm::mat4& GetViewProjectionMatrix();
        const inline glm::vec2& GetPos() { return m_Position; }
        inline float GetZoom() { return m_Zoom; }

private:
        float m_AspectRatio;
        float m_Zoom = 1.0f;
        float m_Rotation = 0.0f;
        glm::vec2 m_Position = glm::vec2(0.0f);
        Graphics::Camera m_Camera;

        bool OnWindowResized(WindowResizeEvent& ev);
        void Recalculate();
};

}  // namespace Borek

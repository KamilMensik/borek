// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/mat4x4.hpp>

#include "Include/Graphics/Camera.h"
#include "Include/Events/WindowEvents.h"

namespace Borek {

struct CameraComponent {
        float aspect_ratio;
        float zoom;
        Graphics::Camera camera;
        bool is_active;

        CameraComponent(float aspect_ratio = 1.6, float zoom = 1.0f);

        const glm::mat4&
        GetViewProjectionMatrix(const TransformComponent& transform);
        bool OnWindowResized(WindowResizeEvent& ev);
        void Recalculate(const TransformComponent& transform);
        inline glm::mat4 GetProjectionMatrix()
        {
                return camera.GetProjectionMatrix();
        }
};

}  // namespace Borek

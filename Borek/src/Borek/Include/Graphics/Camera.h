// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/matrix_float4x4.hpp>

#include "Include/Debug/Assert.h"


namespace Borek {

struct TransformComponent;

namespace Graphics {

class Camera {
public:
        Camera(float left, float right, float bottom, float top);

        inline void Invalidate() { m_Valid = false; }
        inline bool IsValid() { return m_Valid; }
        void Recalculate(const glm::vec3& position, const glm::vec3& rotation);
        void Recalculate(const TransformComponent& transform);
        void SetProjection(float left, float right, float bottom, float top);

        inline const glm::mat4& GetViewProjectionMatrix() const
        {
                BOREK_ENGINE_ASSERT(m_Valid, "Camera isn't recalculated");
                return m_ProjectionViewMatrix;
        }

        inline const glm::mat4& GetProjectionMatrix()
        {
                return m_ProjectionMatrix;
        }

private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionViewMatrix;
        bool m_Valid = false;
};

}  // namespace Graphics
}  // namespace Borek

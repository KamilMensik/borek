// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Include/Graphics/Camera.h"

namespace Borek {
namespace Graphics {

std::string to_s(glm::vec3 vec) {
        return std::format("[{}, {}, {}]", vec.x, vec.y, vec.z);
}

Camera::Camera(float left, float right, float bottom, float top)
{
        SetProjection(left, right, bottom, top);
}

void Camera::SetProjection(float left, float right, float bottom, float top)
{
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        Invalidate();
}

void Camera::Recalculate(const glm::vec3& position, const glm::vec3& rotation)
{
        glm::mat4 transform = glm::inverse(glm::translate(glm::mat4(1.0f), position));

        if (rotation.x != 0) {
                transform = glm::rotate(transform, rotation.x,
                                        glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (rotation.y != 0) {
                transform = glm::rotate(transform, rotation.y,
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotation.z != 0) {
                transform = glm::rotate(transform, rotation.z,
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        m_ViewMatrix = transform;

        m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_Valid = true;
}

}  // namespace Graphics
}  // namespace Borek

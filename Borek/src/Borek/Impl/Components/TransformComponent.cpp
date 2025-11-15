// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include <glm/ext/matrix_transform.hpp>
#include "Include/Components/TransformComponent.h"

namespace Borek {

using Trans = TransformComponent;

Trans&
Trans::Move(float x, float y)
{
        position.x += x;
        position.y += y;
        return *this;
}

Trans&
Trans::Move(const glm::vec2& xy)
{
        position += xy;
        return *this;
}

TransformComponent&
TransformComponent::Rotate(float value)
{
        rotation = value;
        return *this;
}

TransformComponent&
TransformComponent::Scale(float x, float y)
{
        scale = glm::vec2{x, y};
        return *this;
}

TransformComponent&
TransformComponent::Scale(const glm::vec2& value)
{
        scale = value;
        return *this;
}

glm::mat4
Trans::GetTransformMat() const
{
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));

        if (rotation != 0) {
                transform = glm::rotate(transform, glm::radians(rotation),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        return glm::scale(transform, glm::vec3(scale, 1.0f));
}

}  // namespace Borek

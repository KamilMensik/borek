// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Borek {

struct TransformComponent {
        glm::vec2 position;
        float rotation;
        glm::vec2 scale;

        TransformComponent(glm::vec2 pos = glm::vec2(0.0f),
                           float rot = 0,
                           glm::vec2 scale = glm::vec2(1.0f))
                : position(pos), rotation(rot), scale(scale)
        {
        }

        TransformComponent& Move(float x, float y);
        TransformComponent& Move(const glm::vec2& xy);
        TransformComponent& Rotate(float value);
        TransformComponent& Scale(float x, float y);
        TransformComponent& Scale(const glm::vec2& value);

        glm::mat4 GetTransformMat() const;

        void
        operator +=(const TransformComponent& other)
        {
                position += other.position;
                rotation += other.rotation;
                scale *= other.scale;
        }

        void
        operator -=(const TransformComponent& other)
        {
                position -= other.position;
                rotation -= other.rotation;
                scale /= other.scale;
        }
};

}  // namespace Borek

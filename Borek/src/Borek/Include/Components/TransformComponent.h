// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Utils/GeometryUtils.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Borek {

inline glm::vec2 pivot_tof(const glm::i16vec2& vec)
{
        return glm::vec2(vec) * (1.0f / 64.0f);
}

inline glm::vec2 scale_tof(const glm::u16vec2& vec)
{
        return glm::vec2(vec) * (1.0f / 64.0f);
}

inline glm::i16vec2 pivot_toi(const glm::vec2& vec)
{
        return glm::i16vec2(vec * 64.0f);
}

inline glm::u16vec2 scale_toi(const glm::vec2& vec)
{
        return glm::u16vec2(vec * 64.0f);
}

struct TransformComponent {
        glm::vec2 position;
        glm::i16vec2 pivot;
        glm::u16vec2 scale;
        float rotation;

        TransformComponent(const glm::vec2& pos = glm::vec2(0.0f),
                           const glm::i16vec2& pivot = glm::i16vec2(0),
                           const glm::u16vec2& scale = glm::u16vec2(64),
                           float rot = 0.0f)
                : position(pos), pivot(pivot), scale(scale), rotation(rot)
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
                glm::vec2 origin(other.position + pivot_tof(pivot));
                position = Utils::Geometry::rotate_point(
                        origin, other.position + position, other.rotation);

                rotation += other.rotation;
                scale = scale_toi(scale_tof(other.scale) * scale_tof(scale));
        }
};

}  // namespace Borek

// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <concepts>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float2.hpp>

#include "Include/Core.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Graphics/Camera.h"
#include "Include/Base/Component.h"
#include "Include/Objects/Sprite.h"
#include "Include/Base/String.h"
#include "Include/Base/Colors.h"
#include "Include/Scripting/ScriptableObject.h"

namespace Borek {

struct TransformComponent : Component<TransformComponent> {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        TransformComponent(glm::vec3 pos = glm::vec3(0.0f),
                           glm::vec3 rot = glm::vec3(0.0f),
                           glm::vec3 scale = glm::vec3(1.0f))
                : position(pos), rotation(rot), scale(scale) {}

        TransformComponent& Move(float x, float y);
        TransformComponent& Move(float x, float y, float z);
        TransformComponent& Move(const glm::vec2& xy);
        TransformComponent& Move(const glm::vec3& xyz);

        glm::mat4 GetTransformMat() const;
        glm::mat4 GetPixelTransformMat() const;
};

struct SpriteComponent : Component<SpriteComponent> {
        Ref<Sprite> sprite;
        glm::vec4 color;

        SpriteComponent() : color(Colors::WHITE) {}
        SpriteComponent(const Ref<Sprite>& sprite,
                        glm::vec4 color = Colors::WHITE)
                : sprite(sprite), color(color) {}
};

struct TagComponent : Component<TagComponent> {
        String value;

        TagComponent() : value() {}
        TagComponent(const std::string& value) : value(value) {}
};

struct CameraComponent : Component<CameraComponent> {
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

struct ScriptComponent : Component<ScriptComponent> {
        ScriptableObject* instance;

        ScriptComponent() {}
        ScriptComponent(ScriptableObject* so) { instance = so; }

        template <class T> requires std::derived_from<T, ScriptableObject>
        inline void SetScript() { SetScript(new T()); }
        inline void SetScript(ScriptableObject* obj) { instance = obj; }

        inline void Bind(Entity e)
        {
                if (!instance->m_Entity.IsNil()) return;

                instance->m_Entity = e;
                instance->OnCreate();
        }
};

}  // namespace Borek

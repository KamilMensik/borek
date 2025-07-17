// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <concepts>

#include <glm/ext/vector_float3.hpp>
#include "glm/ext/matrix_float4x4.hpp"
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <box2d/id.h>

#include "Include/Core.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Graphics/Camera.h"
#include "Include/Objects/Sprite.h"
#include "Include/Base/String.h"
#include "Include/Base/Colors.h"
#include "Include/Scripting/ScriptableObject.h"
#include "Include/Base/UUID.h"

struct RClass;

namespace Borek {

struct IDComponent {
        UUID id;
        uint32_t ecs_id;

        IDComponent() : id() {}
        IDComponent(UUID id) : id(id) {}
        operator uint64_t() { return id; }
        operator UUID() { return id; }
};

struct TransformComponent {
        glm::vec2 position;
        glm::vec3 rotation;
        glm::vec2 scale;

        TransformComponent(glm::vec2 pos = glm::vec2(0.0f),
                           glm::vec3 rot = glm::vec3(0.0f),
                           glm::vec2 scale = glm::vec2(1.0f))
                : position(pos), rotation(rot), scale(scale) {}

        TransformComponent& Move(float x, float y);
        TransformComponent& Move(const glm::vec2& xy);

        glm::mat4 GetTransformMat(const glm::vec2& offset = glm::vec2()) const;
        glm::mat4 GetPixelTransformMat(const glm::vec2& offset = glm::vec2()) const;
};

struct SpriteComponent {
        Ref<Sprite> sprite;
        glm::vec4 color;

        SpriteComponent() : color(Colors::WHITE) {}
        SpriteComponent(const Ref<Sprite>& sprite,
                        glm::vec4 color = Colors::WHITE)
                : sprite(sprite), color(color) {}
};

struct LineComponent {
        Ref<Sprite> sprite;
        glm::vec4 color;
        float length;


        LineComponent() : color(Colors::WHITE) {}
        LineComponent(const Ref<Sprite>& sprite,
                      glm::vec4 color = Colors::WHITE)
                : sprite(sprite), color(color) {}
};

struct TagComponent {
        String value;

        TagComponent() : value() {}
        TagComponent(const std::string& value) : value(value) {}
};

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

struct ScriptComponent {
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

struct RubyScriptComponent {
        RubyScriptComponent();

        void Initialize(Entity e);
        void OnUpdate(float delta);
        void OnDestroy();

        std::string ruby_class = "";
        uint64_t ruby_instance = 0;
        Entity entity;

        static RClass* s_RubyEntityClass;
};

struct RigidBody2DComponent {
        enum class Type { kStatic, kDynamic, kKinematic };
        Type type;
        bool fixed_rotation;
        b2BodyId runtime_body;

        RigidBody2DComponent() = default;
};

struct BoxCollider2DComponent {
        glm::vec2 offset = glm::vec2();
        glm::vec2 size = glm::vec2(0.5f);

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitution_treshold = 0.5f;
        b2ShapeId runtime_collider;

        BoxCollider2DComponent() = default;
};

}  // namespace Borek
